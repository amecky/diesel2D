#pragma once
#include "..\dxstdafx.h"
#include "..\math\math_types.h"
#include "..\utils\GameMath.h"
#include "..\math\vector.h"
#include "..\math\matrix.h"
#include "Color.h"

namespace ds {

enum QuadAlignment {
	QA_TOP_LEFT,
	QA_CENTERED,
	QA_X_CENTERED,
	QA_Y_CENTERED,
	QA_DEFINED,
	QA_TOP_RIGHT
};

class Quad {

public:
	Vec2 p[4];
	//Vec2 p2;
	//Vec2 p3;
	//Vec2 p4;

	Quad() {
		m_DimX = 20.0f;
		m_DimY = 20.0f;
		m_ScaleX = 1.0f;
		m_ScaleY = 1.0f;
		m_Position = Vec2(0,0);
		m_Rotation = 0.0f;
		m_Active = true;
		update();
	}
	Quad(float dimX,float dimY,float rotation = 0.0f,const Vec2& offset = Vec2(0,0),QuadAlignment alignment = QA_CENTERED) {
		m_DimX = dimX;
		m_DimY = dimY;
		m_ScaleX = 1.0f;
		m_ScaleY = 1.0f;
		m_Position = offset;
		m_Rotation = rotation;
		m_Alignment = alignment;
		m_Active = true;
		update();
	}
	~Quad() {}
	void scale(float dx,float dy) {
		m_ScaleX = dx;
		m_ScaleY = dy;
	}
	const Vec2 getScale() const {
		return Vec2(m_ScaleX,m_ScaleY);
	}
	void rotate(float angle) {		
		m_Rotation = angle;
	}
	const float getRotation() const {
		return m_Rotation;
	}
	const Vec2& getPosition() const {
		return m_Position;
	}
	void translateAbsolute(const Vec2& pos) {
		m_Position = pos;
	}
	void translate(const Vec2& pos) {
		m_Position += pos;		
	}
	void update() {
		float ARRAY[] = {
			// QA Centered
			-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f
		};
		mat3 srt = matrix::srt(m_ScaleX,m_ScaleY,m_Rotation,0.0f,0.0f);
		//float hdx = m_DimX * m_ScaleX * 0.5f;
		//float hdy = m_DimY * m_ScaleY * 0.5f;
		int offset = 0;
		if ( m_Alignment == QA_CENTERED ) {
			offset = 0;
		}
		for ( int i = 0; i < 4; ++i ) {
			p[i].x = ARRAY[offset + i * 2] * m_DimX;
			p[i].y = ARRAY[offset + i * 2 + 1] * m_DimY;
			p[i] = matrix::mul(srt,p[i]);
			p[i] = p[i] + m_Position;
		}
			/*
			p1.x = - hdx;
			p1.y = - hdy;
			p2.x = hdx;
			p2.y = - hdy;
			p3.x = hdx;
			p3.y = hdy;
			p4.x = - hdx;
			p4.y = hdy;	
			*/
		//}
		/*
		else if ( m_Alignment == QA_TOP_LEFT ) {
			p1.x = 0.0f;
			p1.y = 0.0f;
			p2.x = 2.0f * hdx;
			p2.y = 0.0f;
			p3.x = 2.0f * hdx;
			p3.y = 2.0f * hdy;
			p4.x = 0.0f;
			p4.y = 2.0f * hdy;	
		}
		else if ( m_Alignment == QA_TOP_RIGHT ) {
			p1.x = -2.0f * hdx;
			p1.y = 0.0f;
			p2.x = 0.0f;
			p2.y = 0.0f;
			p3.x = 0.0f;
			p3.y = 2.0f * hdy;
			p4.x = -2.0f * hdx;
			p4.y = 2.0f * hdy;	
		}
		else if ( m_Alignment == QA_Y_CENTERED ) {
			p1.x = 0.0f;
			//p1.y = - hdy;
			p1.y = hdy;
			p2.x = 2.0f * hdx;
			//p2.y = - hdy;
			p2.y =  hdy;
			p3.x = 2.0f * hdx;
			//p3.y = hdy;
			p3.y = -hdy;
			p4.x = 0.0f;
			//p4.y = hdy;	
			p4.y = -hdy;	
		}
		else if ( m_Alignment == QA_X_CENTERED ) {
			p1.x = - hdx;
			p1.y = 0.0f;
			p2.x = hdx;
			p2.y = 0.0f;
			p3.x = hdx;
			p3.y = 2.0f * hdy;
			p4.x = - hdx;
			p4.y = 2.0f * hdy;	
		}
		*/
		/*
		if ( m_Rotation != 0 ) {
			vector::rotate(p1,m_Rotation);
			vector::rotate(p2,m_Rotation);
			vector::rotate(p3,m_Rotation);
			vector::rotate(p4,m_Rotation);
		}
		*/
	/*
		for ( int i = 0; i < 4; ++i ) {
			p[i] = matrix::mul(srt,p[i]);
			p[i] = p[i] + m_Position;
		}
		*/
		//p1 += m_Position;
		//p2 += m_Position;
		//p3 += m_Position;
		//p4 += m_Position;
	}

	void update(float scaleX,float scaleY,float rotation,const Vec2& pos) {
		m_ScaleX = scaleX;
		m_ScaleY = scaleY;
		m_Rotation = rotation;
		m_Position = pos;
		update();
	}
	const bool isActive() const {
		return m_Active;
	}
	void setActive(bool active) {
		m_Active = active;
	}
private:
	float m_DimX;
	float m_DimY;
	float m_ScaleX;
	float m_ScaleY;
	float m_Rotation;
	Vec2 m_Position;
	QuadAlignment m_Alignment;
	bool m_Active;
};

class TexturedQuad : public Quad {

public:
	float u1;
	float v1;
	float u2;
	float v2;
	Color color;

	TexturedQuad() : Quad() {
		u1 = v1 = 0.0f;
		u2 = v2 = 1.0f;
		color = Color(1.0f,1.0f,1.0f,1.0f);
	}
	TexturedQuad(float dimX,float dimY,float rotation = 0.0f,const Vec2& offset = Vec2(0,0),const Color& col = Color(1.0f,1.0f,1.0f,1.0f),QuadAlignment alignment = QA_CENTERED) 
		: Quad(dimX,dimY,rotation,offset,alignment) , color(col) , u1(0.0f) , v1(0.0f) , u2(1.0f) , v2(1.0f) {}
	virtual ~TexturedQuad() {}
	void calculateRect(const Rect& rect,uint32 textureSize) {
		math::getTextureCoordinates(rect,textureSize,&u1,&v1,&u2,&v2,true);
	}	
};

}
