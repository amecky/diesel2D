#pragma once
#include "GameMath.h"
/*
namespace ds {
// ---------------------------------------------
// Mat2 methods
// ---------------------------------------------
class Mat2 {

public:
	Vec2 rows[2];

	Mat2();
	Mat2(const Vec2& v1,const Vec2& v2);
	Mat2(float m00,float m01,float m10,float m11);
	~Mat2() {}	
	float operator()(int row,int col);
private:

};

inline Mat2::Mat2() {
	rows[0].x = rows[0].y = rows[1].x = rows[1].y = 0.0f;
}

Vec2 operator * (const Mat2& mat,const Vec2& vec);
Mat2 operator * (const Mat2& mat1,const Mat2& mat2);
Mat2 rotate(const float angle);

// ---------------------------------------------
// Mat3 methods
// ---------------------------------------------
class Mat3 {
public:
	Vec3 rows[3];
	Mat3();
	Mat3(const Vec3& v1,const Vec3& v2,const Vec3& v3);
	Mat3(float m00,float m01,float m02,float m10,float m11,float m12,float m20,float m21,float m22);
};

Mat3 identity3();
float det(const Mat3& m);
Mat3 inverse(const Mat3 &m);
Mat3 operator * (const Mat3 &m, const float x);
Vec2 operator * (const Mat3 &m, const Vec2& v);
Mat3 transformationMatrix(float scaleX,float scaleY,float rotation,const Vec2& pos);
Mat3 operator * (const Mat3 &m, const Mat3 &n);

}
*/