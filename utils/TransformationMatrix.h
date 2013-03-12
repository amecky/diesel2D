#pragma once
#include "GameMath.h"

namespace ds {

class TransformationMatrix {

public:
	TransformationMatrix();
	TransformationMatrix(float scaleX,float scaleY,float rotation,const Vec2& pos);
	~TransformationMatrix();
	Vec2 transform(const Vec2& v);
	void scale(float scaleX,float scaleY);
	void rotate(float angle);
	void transpose(const Vec2& pos);
	void scaleX(float scaleX);
	void scaleY(float scaleY);
	const Vec2& getPosition() const;
	const float getScaleX() const;
	const float getScaleY() const;
	const float getRotation() const;
private:
	void updateMatrix();
	float m_ScaleX;
	float m_ScaleY;
	float m_Rotation;
	Vec2 m_Position;
	Vec3 m_Rows[3];
};

}


