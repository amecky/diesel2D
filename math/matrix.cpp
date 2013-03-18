#include "matrix.h"
#include <math.h>

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
}

}