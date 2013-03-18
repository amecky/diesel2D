#include "Mat.h"
/*
namespace ds {
// ---------------------------------------------
// Mat2 methods
// ---------------------------------------------
Mat2::Mat2(const Vec2& v1,const Vec2& v2) {
	rows[0] = v1;
	rows[1] = v2;
}

Mat2::Mat2(float m00,float m01,float m10,float m11) {
	rows[0].x = m00;
	rows[0].y = m01;
	rows[1].x = m10;
	rows[1].y = m11;
}

Vec2 operator * (const Mat2& mat,const Vec2& vec) {
	float x = mat.rows[0].x * vec.x + mat.rows[0].y * vec.y;
	float y = mat.rows[1].x * vec.x + mat.rows[1].y * vec.y;
	return Vec2(x,y);
}

#define rcDot2(r, c) (mat1.rows[r].x * mat2.rows[0][c] + mat1.rows[r].y * mat2.rows[1][c])

Mat2 operator * (const Mat2& mat1,const Mat2& mat2) {
	float m00 = mat1.rows[0].x * mat2.rows[0].x + mat1.rows[0].y * mat2.rows[1].x;
	float m01 = mat1.rows[0].x * mat2.rows[0].y + mat1.rows[0].y * mat2.rows[1].y;
	float m10 = mat1.rows[1].x * mat2.rows[0].x + mat1.rows[1].y * mat2.rows[1].x;
	float m11 = mat1.rows[1].x * mat2.rows[0].y + mat1.rows[1].y * mat2.rows[1].y;
	return Mat2(m00,m01,m10,m11);
}

Mat2 rotate(const float angle){
	float cosA = cosf(angle), sinA = sinf(angle);
	return Mat2(cosA, -sinA, sinA, cosA);
}

// ---------------------------------------------
// Mat3 methods
// ---------------------------------------------
Mat3::Mat3() {
	rows[0] = Vec3(0,0,0);
	rows[1] = Vec3(0,0,0);
	rows[2] = Vec3(0,0,0);
}

Mat3::Mat3(float m00,float m01,float m02,float m10,float m11,float m12,float m20,float m21,float m22) {
	rows[0] = Vec3(m00,m01,m02);
	rows[1] = Vec3(m10,m11,m12);
	rows[2] = Vec3(m20,m21,m22);
}

Mat3::Mat3(const Vec3& v1,const Vec3& v2,const Vec3& v3) {
	rows[0] = v1;
	rows[1] = v2;
	rows[2] = v3;
}

Mat3 identity3() {
	Mat3 m;
	m.rows[0].x = 1.0f;
	m.rows[1].y = 1.0f;
	m.rows[2].z = 1.0f;
	return m;
}

Mat3 operator * (const Mat3 &m, const float x) {
	return Mat3(m.rows[0] * x, m.rows[1] * x, m.rows[2] * x);
}

float det(const Mat3 &m){
	return 
		m.rows[0].x * (m.rows[1].y * m.rows[2].z - m.rows[2].y * m.rows[1].z) -
		m.rows[0].y * (m.rows[1].x * m.rows[2].z - m.rows[1].z * m.rows[2].x) +
		m.rows[0].z * (m.rows[1].x * m.rows[2].y - m.rows[1].y * m.rows[2].x);
}

Mat3 inverse(const Mat3 &m) {
	float invDet = 1.0f / det(m);

	return Mat3(
		m.rows[1].y * m.rows[2].z - m.rows[1].z * m.rows[2].y, m.rows[2].y * m.rows[0].z - m.rows[0].y * m.rows[2].z, m.rows[0].y * m.rows[1].z - m.rows[1].y * m.rows[0].z,
		m.rows[1].z * m.rows[2].x - m.rows[1].x * m.rows[2].z, m.rows[0].x * m.rows[2].z - m.rows[2].x * m.rows[0].z, m.rows[1].x * m.rows[0].z - m.rows[0].x * m.rows[1].z,
		m.rows[1].x * m.rows[2].y - m.rows[2].x * m.rows[1].y, m.rows[2].x * m.rows[0].y - m.rows[0].x * m.rows[2].y, m.rows[0].x * m.rows[1].y - m.rows[0].y * m.rows[1].x) * invDet;
}

Vec2 operator * (const Mat3& mat,const Vec2& vec) {
	Vec3 v3(vec.x,vec.y,1.0f);
	float x = mat.rows[0].x * v3.x + mat.rows[0].y * v3.y + mat.rows[0].z * v3.z;
	float y = mat.rows[1].x * v3.x + mat.rows[1].y * v3.y + mat.rows[1].z * v3.z;
	return Vec2(x,y);
}

#define rcDot3(r, c) (m.rows[r].x * n.rows[0][c] + m.rows[r].y * n.rows[1][c] + m.rows[r].z * n.rows[2][c])

Mat3 operator * (const Mat3 &m, const Mat3 &n) {
	return Mat3(
		rcDot3(0, 0), rcDot3(0, 1), rcDot3(0, 2),
		rcDot3(1, 0), rcDot3(1, 1), rcDot3(1, 2),
		rcDot3(2, 0), rcDot3(2, 1), rcDot3(2, 2));
}

Mat3 transformationMatrix(float scaleX,float scaleY,float rotation,const Vec2& pos) {
	Mat3 m = identity3();
	m.rows[0].x = scaleX * cos(rotation);
	m.rows[0].y = scaleX * -sin(rotation);
	m.rows[1].x = scaleY * sin(rotation);
	m.rows[1].y = scaleY * cos(rotation);
	m.rows[0].z = pos.x;
	m.rows[1].z = pos.y;
	return m;
}

}
*/