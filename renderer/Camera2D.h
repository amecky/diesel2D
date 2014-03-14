#pragma once
#include "..\math\math_types.h"

namespace ds {

class Camera2D {

public:
	Camera2D(void);
	~Camera2D(void);
	void setPosition(const Vector2f& position,float scale = 1.0f);
	Vector2f transform(const Vector2f& pos);
	Vector2f transformToWorld(const Vector2f& screenPos);
private:
	float m_Scale;
	Vector2f m_Position;
	mat3 m_Matrix;
	mat3 m_WMatrix;
};

}