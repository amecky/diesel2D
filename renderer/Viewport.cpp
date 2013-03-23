#include "Viewport.h"
#include "..\utils\Log.h"

namespace ds {

Viewport::Viewport(int width,int height) : m_Width(width), m_Height(height) {
	m_Position = Vec2(0,0);
	m_MaxX = m_Width;// - gEngine->getWidth();
	m_MaxY = m_Height ;//- gEngine->getHeight();
	m_Delta = Vec2(m_MaxX,m_MaxY);
}

Viewport::~Viewport() {
}

void Viewport::setDimension(int width,int height) {
	LOGC(logINFO,"Viewport") << "Setting dimension to " << width << "x" << height;
	m_Width = width;
	m_Height = height;
	m_MaxX = m_Width;// - gEngine->getWidth();
	m_MaxY = m_Height;// - gEngine->getHeight();
	m_Delta = Vec2(m_MaxX,m_MaxY);
	LOGC(logINFO,"Viewport") << "Delta x: " << m_MaxX<< " y:" << m_MaxY;
}

void Viewport::setPosition(int x,int y) {
	setPosition(Vec2(x,y));	
}

void Viewport::setPosition(const Vec2& pos) {
	m_Position = pos;
	if ( pos.x < 0.0f ) {
		m_Position.x = 0.0f;
	}
	if ( pos.y < 0.0f ) {
		m_Position.y = 0.0f;
	}
	if ( pos.x > m_MaxX ) {
		m_Position.x = m_MaxX;
	}
	if ( pos.y > m_MaxY ) {
		m_Position.y = m_MaxY;
	}
}



};
