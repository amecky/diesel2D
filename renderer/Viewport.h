#pragma once
#include "Vector.h"

namespace ds {

class Viewport {

public:
	Viewport();
	~Viewport(void);
	void setDimension(int width,int height);

	void setDimension(int screenWidth, int screenHeight, int worldWidth, int worldHeight);

	void setPosition(int x,int y);
	void setPosition(const Vector2f& pos);
	const Vector2f& getPosition() const {
		//return m_ViewPos;
		return m_Position;
	}	
	const int getWidth() const { 
		return m_Width; 
	}
	const int getHeight() const { 
		return m_Height; 
	}
	float getPositionX() {
		return m_Position.x;
	}
	float getPositionY() {
		return m_Position.y;
	}
	Vector2f convertToWorld(const Vector2f& screenPos) {
		Vector2f p = screenPos;
		p.x *= m_ViewScaleX;
		p.y *= m_ViewScaleY;
		return p;
	}

private:
	Vector2f m_Position;
	Vector2f m_ViewPos;
	int m_Width;
	int m_Height;
	Vector2f m_Min;
	Vector2f m_Max;
	Vector2f m_ScreenSize;
	Vector2f m_WorldSize;
	float m_ScaleX;
	float m_ScaleY;
	float m_ViewScaleX;
	float m_ViewScaleY;
};

};

