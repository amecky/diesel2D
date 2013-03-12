#include "TransformationMatrix.h"

namespace ds {

TransformationMatrix::TransformationMatrix() {
	m_Rows[0] = Vec3(1,0,0);
	m_Rows[1] = Vec3(0,1,0);
	m_Rows[2] = Vec3(0,0,1);
	m_ScaleX = 1.0f;
	m_ScaleY = 1.0f;
	m_Rotation = 0.0f;
	m_Position = Vec2(0,0);
	updateMatrix();
}

TransformationMatrix::TransformationMatrix(float scaleX,float scaleY,float rotation,const Vec2& pos) {
	m_Rows[0] = Vec3(1,0,0);
	m_Rows[1] = Vec3(0,1,0);
	m_Rows[2] = Vec3(0,0,1);
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
	m_Rotation = rotation;
	m_Position = pos;
	updateMatrix();
}

TransformationMatrix::~TransformationMatrix() {
}

void TransformationMatrix::updateMatrix() {
	m_Rows[0].x = m_ScaleX * cos(m_Rotation);
	m_Rows[0].y = m_ScaleX * -sin(m_Rotation);
	m_Rows[1].x = m_ScaleY * sin(m_Rotation);
	m_Rows[1].y = m_ScaleY * cos(m_Rotation);
	m_Rows[0].z = m_Position.x;
	m_Rows[1].z = m_Position.y;
}

Vec2 TransformationMatrix::transform(const Vec2& v) {
	Vec3 v3(v.x,v.y,1.0f);
	float x = m_Rows[0].x * v3.x + m_Rows[0].y * v3.y + m_Rows[0].z * v3.z;
	float y = m_Rows[1].x * v3.x + m_Rows[1].y * v3.y + m_Rows[1].z * v3.z;
	return Vec2(x,y);
}

void TransformationMatrix::scale(float scaleX,float scaleY) {
	m_ScaleX = scaleX;
	m_ScaleY = scaleY;
	updateMatrix();
}

void TransformationMatrix::rotate(float angle) {
	m_Rotation = angle;
	updateMatrix();
}

void TransformationMatrix::transpose(const Vec2& pos) {
	m_Position = pos;
	updateMatrix();
}

void TransformationMatrix::scaleX(float scaleX) {
	m_ScaleX = scaleX;
	updateMatrix();
}

void TransformationMatrix::scaleY(float scaleY) {
	m_ScaleY = scaleY;
	updateMatrix();
}

const Vec2& TransformationMatrix::getPosition() const {
	return m_Position;
}

const float TransformationMatrix::getScaleX() const {
	return m_ScaleX;
}

const float TransformationMatrix::getScaleY() const {
	return m_ScaleY;
}

const float TransformationMatrix::getRotation() const {
	return m_Rotation;
}

}
