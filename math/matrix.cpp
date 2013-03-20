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

namespace ds {

namespace matrix {

	mat3 indentity() {
		mat3 m;
		m.a.x = 1.0f;
		m.b.y = 1.0f;
		m.c.z = 1.0f;
		return m;
	}

	mat3 srt(float scale,float rotation,float x,float y) {
		float ca = cos(rotation);
		float sa = sin(rotation);
		mat3 m = indentity();
		m.a = Vec3( scale * ca, scale * sa, x);
		m.b = Vec3(scale * -sa, scale * ca, y);
		return m;
	}

	mat3 srt(float scaleX,float scaleY,float rotation,float x,float y) {
		float ca = cos(rotation);
		float sa = sin(rotation);
		mat3 m = indentity();
		m.a = Vec3( scaleX * ca, scaleX * sa, x);
		m.b = Vec3(scaleY * -sa, scaleY * ca, y);
		return m;
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
}

}