#include "matrix.h"
#include <math.h>

ds::mat3 operator * (const ds::mat3& m1,const ds::mat3& m2) {
	ds::mat3 tmp;
	tmp.a.x = m1.a.x * m2.a.x + m1.a.y * m2.b.x + m1.a.z * m2.c.x;
	tmp.a.y = m1.a.x * m2.a.y + m1.a.y * m2.b.y + m1.a.z * m2.c.y;
	tmp.a.z = m1.a.x * m2.a.z + m1.a.y * m2.b.z + m1.a.z * m2.c.z;
	tmp.b.x = m1.b.x * m2.a.x + m1.b.y * m2.b.x + m1.b.z * m2.c.x;
	tmp.b.y = m1.b.x * m2.a.y + m1.b.y * m2.b.y + m1.b.z * m2.c.y;
	tmp.b.z = m1.b.x * m2.a.z + m1.b.y * m2.b.z + m1.b.z * m2.c.z;
	tmp.c.x = m1.c.x * m2.a.x + m1.c.y * m2.b.x + m1.c.z * m2.c.x;
	tmp.c.y = m1.c.x * m2.a.y + m1.c.y * m2.b.y + m1.c.z * m2.c.y;
	tmp.c.z = m1.c.x * m2.a.z + m1.c.y * m2.b.z + m1.c.z * m2.c.z;
	return tmp;
}

ds::Vec3 operator * (const ds::mat3& m,const ds::Vec3& v) {
	ds::Vec3 tmp;
	tmp.x = m.a.x * v.x + m.a.y * v.y + m.a.z * v.z;
	tmp.y = m.b.x * v.x + m.b.y * v.y + m.b.z * v.z;
	tmp.z = m.c.x * v.x + m.c.y * v.y + m.c.z * v.z;
	return tmp;
}

ds::Vec4 operator * (const ds::mat4& m,const ds::Vec4& v) {
	ds::Vec4 tmp;
	tmp.x = m.a.x * v.x + m.a.y * v.y + m.a.z * v.z + m.a.w * v.w;
	tmp.y = m.b.x * v.x + m.b.y * v.y + m.b.z * v.z + m.b.w * v.w;
	tmp.z = m.c.x * v.x + m.c.y * v.y + m.c.z * v.z + m.c.w * v.w;
	tmp.w = m.c.x * v.x + m.c.y * v.y + m.c.z * v.z + m.d.w * v.w;
	return tmp;
}

ds::Vec3 operator * (const ds::mat4& m,const ds::Vec3& v) {
	ds::Vec4 nv(v.x,v.y,v.z,1.0f);
	ds::Vec4 tmp = m * nv;	
	return ds::Vec3(tmp.x,tmp.y,tmp.z);
}

namespace ds {

namespace matrix {

	mat3 identity() {
		mat3 m;
		m.a.x = 1.0f;
		m.b.y = 1.0f;
		m.c.z = 1.0f;
		return m;
	}

	mat3 srt(float scale,float rotation,float x,float y) {
		float ca = cos(rotation);
		float sa = sin(rotation);
		mat3 m = identity();
		m.a = Vec3( scale * ca, scale * sa, x);
		m.b = Vec3(scale * -sa, scale * ca, y);
		return m;
	}

	mat3 srt(float scaleX,float scaleY,float rotation,float x,float y) {
		float ca = cos(rotation);
		float sa = sin(rotation);
		mat3 m = identity();
		m.a = Vec3( scaleX * ca, scaleX * sa, x);
		m.b = Vec3(scaleY * -sa, scaleY * ca, y);
		return m;
	}

	mat3 mat3RotationY(float angle) {
		mat3 tmp = identity();
		tmp.a.x = cos(angle);
		tmp.a.z = sin(angle);
		tmp.c.x = -sin(angle);
		tmp.c.z = cos(angle);		
		return tmp;
	}

	mat3 mat3RotationX(float angle) {
		mat3 tmp = identity();
		tmp.b.y = cos(angle);
		tmp.b.z = -sin(angle);
		tmp.c.y = sin(angle);
		tmp.c.z = cos(angle);
		return tmp;
	}

	mat3 mat3RotationZ(float angle) {
		mat3 tmp = identity();
		tmp.a.x = cos(angle);
		tmp.a.y = -sin(angle);
		tmp.b.x = sin(angle);
		tmp.b.y = cos(angle);
		return tmp;
	}

	Vec3 mul(const mat3& m,const Vec3& v) {
		float m0 = m.a.x * v.x + m.a.y * v.y + m.a.z * v.z;
		float m1 = m.b.x * v.x + m.b.y * v.y + m.b.z * v.z;
		float m2 = m.c.x * v.x + m.c.y * v.y + m.c.z * v.z;
		return Vec3(m0,m1,m2);
	}

	Vec2 mul(const mat3& m,const Vec2& v) {
		// we have to flip y because 0,0 is at the top left corner
		Vec3 tmp(v.x,-v.y,1.0f);
		Vec3 res = mul(m,tmp);
		return Vec2(res.x,res.y);
	}


	mat4 m4identity() {
		mat4 m;
		m.a.x = 1.0f;
		m.b.y = 1.0f;
		m.c.z = 1.0f;
		m.d.w = 1.0f;
		return m;
	}

	mat4 mat4Rotation(const Vec3& v,float angle) {
		float L = (v.x * v.x + v.y * v.y + v.z * v.z);
		float u2 = v.x * v.x;
		float v2 = v.y * v.y;
		float w2 = v.z * v.z; 
		mat4 tmp = m4identity();
		tmp.a.x = (u2 + (v2 + w2) * cos(angle)) / L;
		tmp.a.y = (v.x * v.y * (1 - cos(angle)) - v.z * sqrt(L) * sin(angle)) / L;
		tmp.a.z = (v.x * v.z * (1 - cos(angle)) + v.y * sqrt(L) * sin(angle)) / L;
		tmp.a.w = 0.0f; 

		tmp.b.x = (v.x * v.y * (1 - cos(angle)) + v.z * sqrt(L) * sin(angle)) / L;
		tmp.b.y = (v2 + (u2 + w2) * cos(angle)) / L;
		tmp.b.z = (v.y * v.z * (1 - cos(angle)) - v.x * sqrt(L) * sin(angle)) / L;
		tmp.b.w = 0.0f; 

		tmp.c.x = (v.x * v.z * (1 - cos(angle)) - v.y * sqrt(L) * sin(angle)) / L;
		tmp.c.y = (v.y * v.z * (1 - cos(angle)) + v.x * sqrt(L) * sin(angle)) / L;
		tmp.c.z = (w2 + (u2 + v2) * cos(angle)) / L;
		tmp.c.w = 0.0f; 

		return tmp;
	} 
}

}