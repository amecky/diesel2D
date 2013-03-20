#pragma once
#include "math_types.h"
#include <math.h>

// -------------------------------------------------------
// Vec2 operators
// -------------------------------------------------------
inline ds::Vec2 operator + (const ds::Vec2& u,const ds::Vec2& v) {
	ds::Vec2 tmp;
	tmp.x = u.x + v.x;
	tmp.y = u.y + v.y;
	return tmp;
}

inline ds::Vec2 operator - (const ds::Vec2& u,const ds::Vec2& v) {
	ds::Vec2 tmp;
	tmp.x = u.x - v.x;
	tmp.y = u.y - v.y;
	return tmp;
}

inline ds::Vec2 operator * (const ds::Vec2& u,float s) {
	ds::Vec2 tmp;
	tmp.x = u.x * s;
	tmp.y = u.y * s;
	return tmp;
}

inline ds::Vec2 operator / (const ds::Vec2& u,float s) {
	ds::Vec2 tmp;
	tmp.x = u.x / s;
	tmp.y = u.y / s;
	return tmp;
}

inline ds::Vec2 operator * (float s,const ds::Vec2& u) {
	ds::Vec2 tmp;
	tmp.x = u.x * s;
	tmp.y = u.y * s;
	return tmp;
}

inline bool operator == (const ds::Vec2 &u, const ds::Vec2 &v) {
	float dx = abs(u.x - v.x);
	float dy = abs(u.y - v.y);
	if ( dx > 0.00001f && dy > 0.000001f ) {
		return false;
	}
	return true;
}

inline bool operator != (const ds::Vec2 &u, const ds::Vec2 &v) {
	float dx = abs(u.x - v.x);
	float dy = abs(u.y - v.y);
	if ( dx > 0.00001f || dy > 0.000001f ) {
		return true;
	}
	return false;
}

// -------------------------------------------------------
// Vec3 operators
// -------------------------------------------------------
inline ds::Vec3 operator + (const ds::Vec3& u,const ds::Vec3& v) {
	ds::Vec3 tmp;
	tmp.x = u.x + v.x;
	tmp.y = u.y + v.y;
	tmp.z = u.z + v.z;
	return tmp;
}

inline ds::Vec3 operator - (const ds::Vec3& u,const ds::Vec3& v) {
	ds::Vec3 tmp;
	tmp.x = u.x - v.x;
	tmp.y = u.y - v.y;
	tmp.z = u.z - v.z;
	return tmp;
}

inline ds::Vec3 operator * (const ds::Vec3& u,const ds::Vec3& v) {
	ds::Vec3 tmp;
	tmp.x = u.x * v.x;
	tmp.y = u.y * v.y;
	tmp.z = u.z * v.z;
	return tmp;
}

inline ds::Vec3 operator * (const ds::Vec3& u,float s) {
	ds::Vec3 tmp;
	tmp.x = u.x * s;
	tmp.y = u.y * s;
	tmp.z = u.z * s;
	return tmp;
}

inline bool operator == (const ds::Vec3 &u, const ds::Vec3 &v) {
	float dx = abs(u.x - v.x);
	float dy = abs(u.y - v.y);
	float dz = abs(u.z - v.z);
	if ( dx > 0.00001f && dy > 0.000001f && dz > 0.000001f ) {
		return false;
	}
	return true;
}

inline bool operator != (const ds::Vec3 &u, const ds::Vec3 &v) {
	float dx = abs(u.x - v.x);
	float dy = abs(u.y - v.y);
	float dz = abs(u.z - v.z);
	if ( dx > 0.00001f || dy > 0.000001f || dz > 0.000001f ) {
		return true;
	}
	return false;
}

namespace ds {

	namespace vector {

		void scale(Vec2& v,float s);

		void addScaled(Vec2& v,const Vec2& in,float scale);

		float length(const Vec2& v);

		float sqrLength(const Vec2& v);

		Vec2 cross(const Vec2& v1,const Vec2& v2);

		float dot(const Vec2& v1,const Vec2& v2);

		Vec2 normalize(const Vec2& v);

		void addRadial(Vec2& v,float radius,float angle);

		void rotate(Vec2& v,float angle);

		float getAngle(const Vec2& v1,const Vec2& v2);

		float distance(const Vec2& u,const Vec2& v);		

		void scale(Vec3& v,float s);

		void addScaled(Vec3& v,const Vec3& in,float scale);

		float sqrLength(const Vec3& v);

		float length(const Vec3& v);

		Vec3 normalize(const Vec3& v);

		float dot(const Vec3& v1,const Vec3	& v2);

		Vec3 cross(const Vec3& v1,const Vec3& v2);

	}
}
