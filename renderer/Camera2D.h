#pragma once
#include "..\math\math_types.h"

namespace ds {

class Camera2D {

public:
	Camera2D(void);
	~Camera2D(void);
	void setPosition(const Vec2& position,float scale = 1.0f);
	Vec2 transform(const Vec2& pos);
	Vec2 transformToWorld(const Vec2& screenPos);
private:
	float m_Scale;
	Vec2 m_Position;
	mat3 m_Matrix;
	mat3 m_WMatrix;
};

}