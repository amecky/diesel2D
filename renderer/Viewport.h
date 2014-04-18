#pragma once
#include "Vector.h"
//#include "..\math\math_types.h"

namespace ds {

class Viewport {

public:
	Viewport(int width,int height);
	~Viewport(void);
	void setDimension(int width,int height);
	void setPosition(int x,int y);
	void setPosition(const Vector2f& pos);
	const Vector2f& getPosition() const {
		return m_Position;
	}
	const Vector2f& getDelta() const {
		return m_Delta;
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

private:
	Vector2f m_Position;
	Vector2f m_Delta;
	int m_Width;
	int m_Height;
	int m_MaxX;
	int m_MaxY;
};

};

