#include "vector.h"
#include <math.h>

namespace ds {

namespace vector {

	void rotate(Vec2& v,float angle) {
		float xt = (v.x * cosf(angle)) - (v.y * sinf(angle));
		float yt = (v.y * cosf(angle)) + (v.x * sinf(angle));
		v.x = xt;
		v.y = yt;
	}

	void scale(Vec2& v,float s) {
		v.x *= s;
		v.y *= s;
	}

	void addScaled(Vec2& v,const Vec2& in,float scale) {
		v.x += in.x * scale;
		v.y += in.y * scale;
	}

	float sqrLength(const Vec2& v) {
		return v.x * v.x + v.y * v.y;
	}

	Vec2 cross(const Vec2& v1,const Vec2& v2) {
		float x = v1.x * v2.x;
		float y = v1.y * v2.y;
		return Vec2(x,y);
	}

	float dot(const Vec2& v1,const Vec2& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	float length(const Vec2& v) {
		float d = v.x * v.x + v.y * v.y;
		return sqrt(d);
	}

	Vec2 normalize(const Vec2& v) {
		float l = length(v);
		if ( l != 0.0f ) {
			float x = v.x / l;
			float y = v.y / l;
			return Vec2(x,y);
		}
		return Vec2(0.0f,0.0f);
	}

	void addRadial(Vec2& v,float radius,float angle) {
		v.x += radius * cos(angle);
		v.y += radius * sin(angle);
	}

	float getAngle(const Vec2& v1,const Vec2& v2)  {	
		Vec2 vn1 = vector::normalize(v1);
		Vec2 vn2 = vector::normalize(v2);
		if ( vn1 != vn2 ) {			
			float dot = vector::dot(vn1,vn2);		
			if ( dot < -1.0f ) {
				dot = -1.0f;
			}
			if ( dot > 1.0f ) {
				dot = 1.0f;
			}
			float tmp = acos(dot);
			float cross = (vn1.x * vn2.y) - (vn2.x * vn1.y);
			if ( cross < 0.0f ) {
				tmp = 2.0f * PI - tmp;
			}
			return tmp;		
		}
		else {
			return 0.0f;
		}
	}

	float distance(const Vec2& u,const Vec2& v) {
		float dx = u.x - v.x;
		float dy = u.y - v.y;
		return sqrtf(dx * dx + dy * dy);
	}

	void scale(Vec3& v,float s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
	}

	void addScaled(Vec3& v,const Vec3& in,float scale) {
		v.x += in.x * scale;
		v.y += in.y * scale;
		v.z += in.z * scale;
	}

	float sqrLength(const Vec3& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	float length(const Vec3& v) {
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	Vec3 normalize(const Vec3& v) {
		float l = length(v);
		if ( l != 0.0f ) {
			float x = v.x / l;
			float y = v.y / l;
			float z = v.z / l;
			return Vec3(x,y,z);
		}
		return Vec3(0.0f,0.0f,0.0f);
	}

	float dot(const Vec3& v1,const Vec3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	Vec3 cross(const Vec3& v1,const Vec3& v2) {
		float x= v1.y * v2.z - v1.z * v2.y;
		float y= v1.z * v2.x - v1.x * v2.z; 
		float z= v1.x * v2.y - v1.y * v2.x;
		return Vec3(x,y,z);
	}

	// -------------------------------------------------------
	// Compute normal vector for 3 given vertices
	// -------------------------------------------------------
	Vec3 computeNormal(const Vec3& v1,const Vec3& v2,const Vec3& v3) {
		Vec3 u = v2 - v1;
		Vec3 v = v3 - v1;
		Vec3 ret = cross(u,v);
		return normalize(ret);
	}

}

}