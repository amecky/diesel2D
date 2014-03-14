#include "NewSpriteBatch.h"
#include "..\renderer\vertex_types.h"
#include "..\utils\Profiler.h"
//#include "..\utils\PlainTextReader.h"
//#include "..\pipeline\PAKReader.h"
//#include "..\pipeline\PAKWriter.h"
//#include "..\utils\FileUtils.h"
#include "..\renderer\shader.h"
#include "..\sprites\SpriteObject.h"
#include "..\renderer\Renderer.h"
#include "..\utils\Log.h"
#include "..\math\GameMath.h"

namespace ds {

const uint32 QUAD_SIZE = 144;
const uint32 INDEX_SIZE = 4;
const uint32 PTC_SIZE = 36;
const uint32 MAX_QUADS = 512;

const float VP_ARRAY[] = {
	-0.5f,0.5f, 0.5f,0.5f, 
	0.5f,-0.5f, -0.5f,-0.5f
};
// ------------------------------------------------------------
//
// ------------------------------------------------------------
NewSpriteBatch::NewSpriteBatch(Renderer* renderer) : m_Renderer(renderer) 
	, m_VertexCounter(0) , m_Index(0) , m_Rendering(false) , m_CurrentTextureID(-1) {
	m_MaxVertices = 4 * MAX_QUADS;
	m_MaxIndices = 6 * MAX_QUADS;
	m_BufferSize = QUAD_SIZE * m_MaxVertices + INDEX_SIZE * m_MaxIndices;
	m_StartIndices = QUAD_SIZE * m_MaxVertices;
	m_DataBuffer = new char[m_BufferSize];

	//Vector2f textureSize = m_Renderer->getTextureSize(textureID);
	//m_TextureWidth = textureSize.x;
	//m_TextureHeight = textureSize.y;
	m_BlendState = m_Renderer->getDefaultBlendState();
	m_Handle = renderer->createBufferHandle(PT_TRI,VD_PTC,GBT_BOTH,true);
	LOGC(logINFO,"SpriteBatch") << "creating new SpriteBatch - buffer size " << m_BufferSize << " start index buffer " << m_StartIndices;
	m_ShaderID = shader::createPTCShader(m_Renderer,-1);
	//LOGC(logINFO,"SpriteBatch") << "tw " << m_TextureWidth << " th " << m_TextureHeight;
	LOGC(logINFO,"SpriteBatch") << "max vertices: " << m_MaxVertices;
	// fill up all indices

	char* indexBuffer = m_DataBuffer + m_StartIndices;
	for ( uint32 i = 0; i < MAX_QUADS;++i ) {
		(*(uint32*)indexBuffer) = i * 4 + 0;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 1;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 2;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 2;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 3;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 0;
		indexBuffer += INDEX_SIZE;
	}
}


NewSpriteBatch::~NewSpriteBatch() {
	LOGC(logINFO,"SpriteBatch") << "destructing SpriteBatch";	
	delete m_DataBuffer;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void NewSpriteBatch::draw(float x,float y,int textureID,const Rect& textureRect,float rotation,float scaleX,float scaleY,const Color& color,const Vector2f& center) {
	draw(Vector2f(x,y),textureID,textureRect,rotation,scaleX,scaleY,color,center);	
}

void NewSpriteBatch::draw(const SpriteObject& spriteObject) {
	if ( spriteObject.isActive()) {
		if ( (m_VertexCounter + 4) >= m_MaxVertices || spriteObject.getTextureID() != m_CurrentTextureID ) {
			m_CurrentTextureID = spriteObject.getTextureID();
			end();
			begin();
		}
		char* buffer = m_DataBuffer + QUAD_SIZE * m_Index;
		float cx = m_Renderer->getWidth() * 0.5f;
		float cy = m_Renderer->getHeight() * 0.5f;
		const Color& clr = spriteObject.getColor();
		for ( int i = 0; i < 4; ++i ) {		
			(*(SpritePlane*)buffer).v[i].uv = spriteObject.getUV(i);
			const Vector3f& p = spriteObject.getVertex(i);
			(*(SpritePlane*)buffer).v[i].x = p.x - cx;
			(*(SpritePlane*)buffer).v[i].y = p.y - cy;
			(*(SpritePlane*)buffer).v[i].z = p.z;
			(*(SpritePlane*)buffer).v[i].color = clr;
		}	
		++m_Index;
		m_VertexCounter += 4;
	}
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void NewSpriteBatch::draw(const Vector2f& pos,int textureID,const Rect& textureRect,float rotation,float scaleX,float scaleY,const Color& color,const Vector2f& center) {
	if ( m_VertexCounter < m_MaxVertices ) {
		float u1,v1,u2,v2;
		ds::math::getTextureCoordinates(textureRect,m_TextureWidth,m_TextureHeight,&u1,&v1,&u2,&v2,true);
		float dimX = textureRect.width();
		float dimY = textureRect.height();
		float dx = dimX * 0.5f;
		float dy = dimY * 0.5f;
		char* buffer = m_DataBuffer + QUAD_SIZE * m_Index;
		(*(SpritePlane*)buffer).v[0].uv = Vector2f(u1,v1);
		(*(SpritePlane*)buffer).v[1].uv = Vector2f(u2,v1);
		(*(SpritePlane*)buffer).v[2].uv = Vector2f(u2,v2);
		(*(SpritePlane*)buffer).v[3].uv = Vector2f(u1,v2);

		Vector2f cor = pos;
		cor = cor - Vector2f(m_Renderer->getWidth() * 0.5f,m_Renderer->getHeight() * 0.5f);
		Vector2f p(0,0);
		for ( int i = 0; i < 4; ++i ) {
			p.x = VP_ARRAY[i * 2] * dimX;
			p.y = VP_ARRAY[i * 2 + 1] * dimY;
			p = p - center;
			Vector2f np = vector::srt(cor,p,scaleX,scaleY,rotation);		
			(*(SpritePlane*)buffer).v[i].x = np.x;
			(*(SpritePlane*)buffer).v[i].y = np.y;
			(*(SpritePlane*)buffer).v[i].z = 0.0f;
			(*(SpritePlane*)buffer).v[i].color = color;
		}	
		++m_Index;
		m_VertexCounter += 4;
	}
}


// -------------------------------------------------------
// Begin
// -------------------------------------------------------
void NewSpriteBatch::begin() {
	m_Index = 0;
	m_VertexCounter = 0;
	m_Rendering = true;
}
// -------------------------------------------------------
// Prepare buffer
// -------------------------------------------------------
void NewSpriteBatch::prepareBuffer() {
	if ( m_VertexCounter > 0 ) {
		WORD* ib;
		PTCVertex* vb;
		int vc = m_VertexCounter;
		int ic = m_VertexCounter / 4 * 6;
		m_Renderer->lockBuffer(m_Handle,vc,ic,(float**)&vb,(void**)&ib);
		char* vBuffer = m_DataBuffer;
		for ( uint32 i = 0; i < m_Index;++i ) {
			for ( int j = 0; j < 4; ++j ) {
				*vb = (*(SpritePlane*)vBuffer).v[j];
				++vb;
			}
			vBuffer += QUAD_SIZE;
			// FIXME: put it at the end
			m_Renderer->getDrawCounter().addSprite();
		}		
		uint32 indexCounter = m_VertexCounter / 4 * 6;
		char* iBuffer = m_DataBuffer + m_StartIndices;
		for ( uint32 i = 0; i < indexCounter; ++i ) {
			*ib = (*(uint32*)iBuffer);
			++ib;
			iBuffer += INDEX_SIZE;
		}
		m_Renderer->unlockBuffer(m_Handle);		
	}
	
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void NewSpriteBatch::end() {
	if ( m_VertexCounter > 0 ) {
		PR_START("SpriteBatch")	
		prepareBuffer();
		m_Renderer->setCurrentShader(m_ShaderID);
		m_Renderer->setTexture(m_CurrentTextureID,0);
		m_Renderer->setTexture(m_ShaderID,"gTex",m_CurrentTextureID);
		m_Renderer->setBlendState(m_BlendState);
		m_Renderer->drawBuffer(m_Handle,m_CurrentTextureID);
		PR_END("SpriteBatch")
	}	
}

}
