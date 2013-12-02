#include "SpriteBatch.h"
#include "..\renderer\vertex_types.h"
#include "..\utils\Profiler.h"
#include "..\utils\PlainTextReader.h"
#include "..\pipeline\PAKReader.h"
#include "..\pipeline\PAKWriter.h"
#include "..\utils\FileUtils.h"
#include "..\renderer\shader.h"

namespace ds {

const uint32 QUAD_SIZE = 144;
const uint32 INDEX_SIZE = 4;
const uint32 PTC_SIZE = 36;

const float VP_ARRAY[] = {
	-0.5f,0.5f, 0.5f,0.5f, 
	0.5f,-0.5f, -0.5f,-0.5f
};
// ------------------------------------------------------------
//
// ------------------------------------------------------------
SpriteBatch::SpriteBatch(Renderer* renderer,uint32 maxQuads,int textureID) : m_Renderer(renderer) , m_VertexCounter(0) , m_Index(0) , m_TextureID(textureID) {
	m_MaxVertices = 4 * maxQuads;
	m_MaxIndices = 6 * maxQuads;
	m_BufferSize = QUAD_SIZE * m_MaxVertices + INDEX_SIZE * m_MaxIndices;
	m_StartIndices = QUAD_SIZE * m_MaxVertices;
	m_DataBuffer = new char[m_BufferSize];

	Vec2 textureSize = m_Renderer->getTextureSize(textureID);
	m_TextureWidth = textureSize.x;
	m_TextureHeight = textureSize.y;
	m_BlendState = m_Renderer->getDefaultBlendState();
	m_Handle = renderer->createBufferHandle(PT_TRI,VD_PTC,GBT_BOTH,true);
	LOGC(logINFO,"SpriteBatch") << "creating new SpriteBatch - buffer size " << m_BufferSize << " start index buffer " << m_StartIndices;
	m_ShaderID = shader::createPTCShader(m_Renderer,textureID);
	LOGC(logINFO,"SpriteBatch") << "tw " << m_TextureWidth << " th " << m_TextureHeight;
	// fill up all indices

	char* indexBuffer = m_DataBuffer + m_StartIndices;
	for ( uint32 i = 0; i < maxQuads;++i ) {
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


SpriteBatch::~SpriteBatch() {
	LOGC(logINFO,"SpriteBatch") << "destructing SpriteBatch";	
	delete m_DataBuffer;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void SpriteBatch::draw(float x,float y,const Rect& textureRect,float rotation,float scaleX,float scaleY,const Color& color,const Vec2& center) {
	Sprite sp;
	sp.position = ds::Vec2(x,y);
	sp.textureRect = textureRect;
	sp.rotation = rotation;
	sp.scaleX = scaleX;
	sp.scaleY = scaleY;
	sp.color = color;
	sp.center = center;
	draw(sp);
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void SpriteBatch::draw(const Vec2& pos,const Rect& textureRect,float rotation,float scaleX,float scaleY,const Color& color,const Vec2& center) {
	Sprite sp;
	sp.position = pos;
	sp.textureRect = textureRect;
	sp.rotation = rotation;
	sp.scaleX = scaleX;
	sp.scaleY = scaleY;
	sp.color = color;
	sp.center = center;
	draw(sp);
}
// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void SpriteBatch::draw(const Sprite& sprite) {
	if ( m_VertexCounter < m_MaxVertices ) {
		float u1,v1,u2,v2;
		ds::math::getTextureCoordinates(sprite.textureRect,m_TextureWidth,m_TextureHeight,&u1,&v1,&u2,&v2,true);
		float dimX = sprite.textureRect.width();
		float dimY = sprite.textureRect.height();
		float dx = dimX * 0.5f;
		float dy = dimY * 0.5f;
		char* buffer = m_DataBuffer + QUAD_SIZE * m_Index;
		(*(SpritePlane*)buffer).v[0].uv = Vec2(u1,v1);
		(*(SpritePlane*)buffer).v[1].uv = Vec2(u2,v1);
		(*(SpritePlane*)buffer).v[2].uv = Vec2(u2,v2);
		(*(SpritePlane*)buffer).v[3].uv = Vec2(u1,v2);

		Vec2 cor = sprite.position;
		cor = cor - Vec2(m_Renderer->getWidth() * 0.5f,m_Renderer->getHeight() * 0.5f);
		Vec2 p(0,0);
		for ( int i = 0; i < 4; ++i ) {
			p.x = VP_ARRAY[i * 2] * dimX;
			p.y = VP_ARRAY[i * 2 + 1] * dimY;
			p = p - sprite.center;
			Vec2 np = vector::srt(cor,p,sprite.scaleX,sprite.scaleY,sprite.rotation);		
			(*(SpritePlane*)buffer).v[i].x = np.x;
			(*(SpritePlane*)buffer).v[i].y = np.y;
			(*(SpritePlane*)buffer).v[i].z = 0.0f;
			(*(SpritePlane*)buffer).v[i].color = sprite.color;
		}	
		++m_Index;
		m_VertexCounter += 4;
		m_Renderer->getDrawCounter().addSprite();
	}
}

// -------------------------------------------------------
// Begin
// -------------------------------------------------------
void SpriteBatch::begin() {
	m_Index = 0;
	m_VertexCounter = 0;
}
// -------------------------------------------------------
// Prepare buffer
// -------------------------------------------------------
void SpriteBatch::prepareBuffer() {
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
void SpriteBatch::end() {
	PR_START("SpriteBatch")
	prepareBuffer();
	m_Renderer->setCurrentShader(m_ShaderID);
	m_Renderer->setTexture(m_TextureID,0);
	m_Renderer->setBlendState(m_BlendState);
	if ( m_VertexCounter > 0 ) {	
		m_Renderer->drawBuffer(m_Handle,m_TextureID);
	}
	PR_END("SpriteBatch")
}

}
