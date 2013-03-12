#include "SpriteNode.h"
#include "..\utils\Profiler.h"
#include "..\utils\Log.h"

namespace ds {

SpriteNode::SpriteNode(const char* name,Renderer* renderer,int mtrl,SpriteColorMode mode) 
	: Node(name,renderer,mtrl) , m_ColorMode(mode) {	
	LOGC(logINFO,"SpriteNode") << "creating new SpriteNode "  << name;
	m_Positions = new Vec2[MAX_SPRITES];
	m_Offsets = new RECT[MAX_SPRITES];
	m_Visible = new bool[MAX_SPRITES];
	m_Colors = new Color[MAX_SPRITES];
	m_Rotations = new float[MAX_SPRITES];
	m_Scales = new Vec2[MAX_SPRITES];
	m_Centers = new Vec2[MAX_SPRITES];
	m_Matrix = new Matrix[MAX_SPRITES];
	m_Dirty = new bool[MAX_SPRITES];
	m_Counter = 0;
	m_IgnoreViewport = false;
	HR(D3DXCreateSprite( renderer->getDevice(), &pSprite ));
}

SpriteNode::~SpriteNode() {
	delete m_Dirty;
	delete m_Matrix;
	delete m_Centers;
	delete m_Scales;
	delete m_Rotations;
	delete m_Colors;
	delete m_Visible;
	delete m_Offsets;
	delete m_Positions;
	SAFE_RELEASE(pSprite);
}

//--------------------------------------------------------------
// This method will add a sprite to the batch node.
// The color will be set to white. Calls addSprite with
// position and rect and color white
//
// [in] position - a Vec3 to define the position of the sprite
// [in] offset - the texture rectangle 
// [out] SHandle that was created
//--------------------------------------------------------------
SHandle SpriteNode::add(const Vec2& position,const Rect& offset) {	
	int idx = m_Counter;
	m_Positions[idx] = position;
	m_Offsets[idx] = offset;
	m_Visible[idx] = true;
	m_Colors[idx] = Color(1.0f,1.0f,1.0f,1.0f);
	m_Rotations[idx] = 0.0f;
	m_Scales[idx] = Vec2(1,1);
	m_Centers[idx]= Vec2((offset.right-offset.left)/2,(offset.bottom - offset.top)/2);
	m_Matrix[idx] = Matrix();
	m_Dirty[idx] = true;
	++m_Counter;
	return SHandle(idx);
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SpriteNode::setColor(const SHandle& handle,const Color& color) {
	m_Colors[handle.m_Index] = color;
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SpriteNode::setScale(const SHandle& handle,const Vec2& pos) {
	m_Scales[handle.m_Index] = pos;
	m_Dirty[handle.m_Index] = true;
}

void SpriteNode::setScale(const SHandle& handle,float x,float y) {
	m_Scales[handle.m_Index] = ds::Vec2(x,y);
	m_Dirty[handle.m_Index] = true;
}

void SpriteNode::setRotation(const SHandle& handle,float angle) {
	m_Rotations[handle.m_Index] = angle;
	m_Dirty[handle.m_Index] = true;
}

float SpriteNode::getRotation(const SHandle& handle) {
	return m_Rotations[handle.m_Index];
}
//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SpriteNode::setPosition(const SHandle& handle,const Vec2& pos) {
	m_Positions[handle.m_Index] = pos;
	m_Dirty[handle.m_Index] = true;
}

Vec2& SpriteNode::getPosition(const SHandle& handle) const {
	return m_Positions[handle.m_Index];
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SpriteNode::setVisible(const SHandle& handle,const bool visible) {
	m_Visible[handle.m_Index] = visible;
}

bool SpriteNode::isVisible(const SHandle& handle) const {
	return m_Visible[handle.m_Index];
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SpriteNode::setOffset(const SHandle& handle,const Rect& offset) {
	m_Offsets[handle.m_Index] = offset;
	m_Dirty[handle.m_Index] = true;
}

void SpriteNode::setOffset(const SHandle& handle,int top,int left,int width,int height) {
	Rect offset(top,left,width,height);
	m_Offsets[handle.m_Index] = offset;
	m_Dirty[handle.m_Index] = true;
}

Rect SpriteNode::getOffset(const SHandle& handle) const {
	return m_Offsets[handle.m_Index];
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
void SpriteNode::clear() {
	m_Counter = 0;
}

//--------------------------------------------------------------
// Internal method called by the rendering system
//--------------------------------------------------------------
void SpriteNode::prepareRendering() {
	//PR_START(getName().c_str());	
	if ( m_Counter > 0 ) {
		//Viewport *vp = gEngine->getViewport();
		Vec2 vpPos(0,0);// = vp->getPosition();	
		Vec2 vCenter( 0.0f, 0.0f );	
		Vec2 *pos = m_Positions;
		bool* visible = m_Visible;
		Matrix* mat = m_Matrix;
		float* rotation = m_Rotations;
		Vec2* scaling = m_Scales;
		RECT* offset = m_Offsets;
		bool* dirty = m_Dirty;
		for ( int i = 0; i < m_Counter ; ++i) {						
			if ( *visible && *dirty ) {
				if ( !m_IgnoreViewport ) {
					pos->x = pos->x - vpPos.x;
					pos->y = pos->y - vpPos.y;
				}
				vCenter = Vec2((offset->right-offset->left)*0.5f*scaling->x,(offset->bottom - offset->top)*0.5f*scaling->y);			
				//Vec2 trans = Vec2(pos->x,pos->y);
				D3DXMatrixTransformation2D(mat,NULL,0.0,scaling,&vCenter,*rotation,pos);
				//D3DXMatrixTransformation2D(mat,NULL,0.0,scaling,NULL,*rotation,&trans);
				*dirty = false;
			}
			++pos;
			++visible;
			++mat;
			++rotation;
			++scaling;
			++offset;
			++dirty;
		}
	}
	//PR_END(getName().c_str());
}

//--------------------------------------------------------------
// Internal method called by the rendering system
//--------------------------------------------------------------
void SpriteNode::draw() {
	PR_START(m_NodeName);
	if ( m_Counter > 0 ) {
		// FIXME: add getTextureFromMaterial
		LPDIRECT3DTEXTURE9 tex = 0;//m_Renderer->getDirectTexture(m_Material->textures[0]);
		bool* visible = m_Visible;
		Matrix* mat = m_Matrix;
		RECT* offset = m_Offsets;
		Color* color = m_Colors;
		pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		/*
		if ( m_ColorMode == SCM_ADD ) {
			m_Renderer->getDevice()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_ADD );
		}
		else if ( m_ColorMode == SCM_MODULATE ) {
			m_Renderer->getDevice()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		}
		else if ( m_ColorMode == SCM_SUBTRACT ) {
			m_Renderer->getDevice()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
		}
		*/
		for ( int i = 0; i < m_Counter; ++i) {				
			if ( *visible ) {			
				pSprite->SetTransform(mat);
				pSprite->Draw(tex,offset,NULL,NULL,*color);
				m_Renderer->getDrawCounter().addSprite();
			}
			++mat;
			++offset;
			++visible;
			++color;
		}
		pSprite->End();		
	}
	PR_END(m_NodeName);
}

void SpriteNode::debug() {
	LOGC(logINFO,"World") << "   SpriteBatchNode -> Sprites: " << m_Counter;
}

};

