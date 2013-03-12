#pragma once
#include "..\utils\GameMath.h"

namespace ds {

class Viewport {

public:
	Viewport(int width,int height);
	~Viewport(void);
	void setDimension(int width,int height);
	void setPosition(int x,int y);
	void setPosition(const Vec2& pos);
	const Vec2& getPosition() const {
		return m_Position;
	}
	const Vec2& getDelta() const {
		return m_Delta;
	}
	const int getWidth() const { 
		return m_Width; 
	}
	const int getHeight() const { 
		return m_Height; 
	}
private:
	Vec2 m_Position;
	Vec2 m_Delta;
	int m_Width;
	int m_Height;
	int m_MaxX;
	int m_MaxY;
};

};

