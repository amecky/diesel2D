#include "SpriteObject.h"
#include "..\math\GameMath.h"
#include "SpriteAnimation.h"

namespace ds {


const float WO_VP_ARRAY[] = {
	-0.5f,0.5f, 0.5f,0.5f, 
	0.5f,-0.5f, -0.5f,-0.5f
};

SpriteObject::SpriteObject() : m_Timer(0.0f) , m_Active(true) , m_Radius(0.0f) , m_Position(0.0f,0.0f) 
	, m_Index(0) , m_ID(0) , m_Color(255,255,255,255) , m_Angle(0.0f) , m_Size(1.0f,1.0f) , dimX(10.0f) , dimY(10.0f) 
	, m_Velocity(0.0f,0.0f) , m_Target(0.0f,0.0f) , m_TextureID(0) {
}

void SpriteObject::setTextureRect(const Rect& r,int textureID) {
	float u1,u2,v1,v2;
	math::getTextureCoordinates(r,1024,&u1,&v1,&u2,&v2);
	dimX = r.width();
	dimY = r.height();
	m_UV[0] = Vector2f(u1,v1);
	m_UV[1] = Vector2f(u2,v1);
	m_UV[2] = Vector2f(u2,v2);
	m_UV[3] = Vector2f(u1,v2);
	setPosition(m_Position);
	m_TextureID = textureID;
}

void SpriteObject::update(float elapsed) {
	m_Timer += elapsed;
	for ( size_t i = 0; i < m_Animations.size(); ++i ) {
		m_Animations[i]->update(m_Timer,this);
	}
}

SpriteObject::~SpriteObject(void) {
}

void SpriteObject::setPosition(const Vector2f& pos) {
	m_Position = pos;
	Vector2f cor = m_Position;
	Vector2f p(0,0);
	for ( int i = 0; i < 4; ++i ) {
		p.x = WO_VP_ARRAY[i * 2] * dimX;
		p.y = WO_VP_ARRAY[i * 2 + 1] * dimY;
		Vector2f np = ds::vector::srt(cor,p,m_Size.x,m_Size.y,m_Angle);				
		m_Vertices[i].x = np.x;
		m_Vertices[i].y = np.y;
		m_Vertices[i].z = 0.0f;
	}	

}

}

