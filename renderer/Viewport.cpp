#include "Viewport.h"
#include "..\utils\Log.h"
#include "..\dxstdafx.h"

namespace ds {

	Viewport::Viewport() : m_Position(0, 0) {//, m_Width(1024) , m_Height(768) {
		setDimension(1024, 768,1024,768);
	}

	Viewport::~Viewport() {}

	void Viewport::setDimension(int width,int height) {
		//LOGC("Viewport") << "Setting dimension to " << width << "x" << height;
		setDimension(width, height, width, height);
	}

	void Viewport::setDimension(int screenWidth, int screenHeight, int worldWidth, int worldHeight) {
		m_ScreenSize = Vector2f(screenWidth, screenHeight);
		m_WorldSize = Vector2f(worldWidth, worldHeight);
		m_ScaleX = m_ScreenSize.x / m_WorldSize.x;
		m_ScaleY = m_ScreenSize.y / m_WorldSize.y;
		m_ViewScaleX = m_WorldSize.x / m_ScreenSize.x;
		m_ViewScaleY = m_WorldSize.y / m_ScreenSize.y;
		//LOG << "Viewport scale.x : " << m_ScaleX << " scale.y : " << m_ScaleY;
		float deltaX = (m_WorldSize.x - m_ScreenSize.x) * 0.5f;
		float deltaY = (m_WorldSize.y - m_ScreenSize.y) * 0.5f;
		//LOG << "delta " << deltaX << " " << deltaY;
		m_Max = Vector2f(m_WorldSize.x * 0.5f + deltaX, m_WorldSize.y * 0.5f + deltaY);
		m_Min = Vector2f(m_WorldSize.x * 0.5f - deltaX,m_WorldSize.y * 0.5f - deltaY);
		//LOG << "max: " << DBG_V2(m_Max);
		//LOG << "min: " << DBG_V2(m_Min);
	}

	void Viewport::setPosition(int x,int y) {
		setPosition(Vector2f(x,y));	
	}

	void Viewport::setPosition(const v2& pos) {
		m_Position = pos;
		clamp(&m_Position, m_Min, m_Max);	
		m_ViewPos.x = m_Position.x * m_ScaleX;
		m_ViewPos.y = m_Position.y * m_ScaleY;
	}

	const bool Viewport::isInside(const Vector2f& pos,const Vector2f& extent) {
		if ( pos.x < extent.x * 0.5f || pos.x > ( m_WorldSize.x - extent.x * 0.5f )) {
			return false;
		}
		if ( pos.y < extent.y * 0.5f || pos.y > ( m_WorldSize.y - extent.y * 0.5f )) {
			return false;
		}
		return true;
	}

};
