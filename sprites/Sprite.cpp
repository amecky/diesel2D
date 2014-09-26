#include "Sprite.h"
#include "..\math\GameMath.h"
#include "..\compiler\Converter.h"

namespace ds {


const float WO_VP_ARRAY[] = {
	-0.5f,0.5f, 0.5f,0.5f, 
	0.5f,-0.5f, -0.5f,-0.5f
};

Sprite::Sprite() : m_Timer(0.0f) , m_Active(true) , m_Radius(0.0f) , m_Position(0.0f,0.0f) 
, m_Index(0), m_ID(0), m_Color(255, 255, 255, 255), m_Angle(0.0f), m_Size(1.0f, 1.0f), dimX(10.0f), dimY(10.0f), dimension(10,10)
	, m_Velocity(0.0f,0.0f) , m_Target(0.0f,0.0f) , m_TextureID(0) , m_UserValue(0) , m_Elapsed(0.0f) , m_Delay(0.0f) {
}

Sprite::~Sprite(void) {
}

void Sprite::setTextureRect(const Rect& r,int textureWidth,int textureHeight,int textureID) {
	float u1,u2,v1,v2;
	math::getTextureCoordinates(r,textureWidth,textureHeight,&u1,&v1,&u2,&v2);
	dimX = r.width();
	dimY = r.height();
	m_UV[0] = Vector2f(u1,v1);
	m_UV[1] = Vector2f(u2,v1);
	m_UV[2] = Vector2f(u2,v2);
	m_UV[3] = Vector2f(u1,v2);
	setPosition(m_Position);
	m_TextureID = textureID;
	uv = math::getTextureCoordinates(r);
	dimension = Vector2f(dimX, dimY);
}

void Sprite::update(float elapsed) {
	tick(elapsed);
}

void Sprite::setPosition(const Vector2f& pos) {
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

void Sprite::load(BinaryLoader* loader) {
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == CHNK_SPRITE ) {		
			loader->read(&m_TextureID);
			Vector2f pos;
			loader->read(&pos);
			Rect r;
			loader->read(&r);
			setTextureRect(r,1024,1024,m_TextureID);
			loader->read(&m_Size);
			loader->read(&m_Color);
			float rotation = 0.0f;
			loader->read(&rotation);
			m_Angle = DEGTORAD(rotation);
			resetTimer();
			setPosition(pos);			
		}
		loader->closeChunk();
	}		
}

void Sprite::run(Script& script) {
	script.context.connect("position",&m_Position);
	script.context.connect("scale",&m_Size);
	script.context.connect("elapsed",&m_Elapsed);
	script.context.connect("timer",&m_Timer);
	script.run();
	setPosition(m_Position);
}

}

