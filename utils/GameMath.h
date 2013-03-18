#pragma once
#include "..\dxstdafx.h"
#include "Color.h"
#include "..\math\math_types.h"
#include "..\math\vector.h"

namespace ds {

// ---------------------------------------------
// Old RND class
// ---------------------------------------------
class RND {

public:
	static int generate(int start,int end);
	static Vec3 getRandomVector();
	static float generate( float fMin, float fMax );
};

const Vec3 ZeroVec = Vec3(0.0f,0.0f,0.0f);

class MathHelper {

public:
	static Vec2 getRadialVelocity(float angle,float vel);
	static float getAngle(float x1,float y1,float x2,float y2);
	static float getAngle(const Vec2& v1,const Vec2& v2);
};

inline Vec2 MathHelper::getRadialVelocity(float angle,float velocity) {
	float vx = (float)cos(DEGTORAD(angle))*velocity;
	float vy = (float)sin(DEGTORAD(angle))*velocity;
	return Vec2(vx,vy);
}

inline float MathHelper::getAngle(float x1,float y1,float x2,float y2) {
	double x = x2 - x1;
	double y = y2 - y1;	
	double ang = atan2(y,x);
	return (float)ang;
}

inline float MathHelper::getAngle(const Vec2& v1,const Vec2& v2) {	
	//if ( v1 != v2 ) {
		Vec2 vn1 = vector::normalize(v1);
		Vec2 vn2 = vector::normalize(v2);
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
			tmp = 2.0f * D3DX_PI - tmp;
		}
		return tmp;
		/*
		Vec2 vc = vn1.Cross(vn2);
		float mag = vc.Length();

		float ab = atan2(mag,dot);
		if ( dot < 0.0f ) {
			ab = 2.0f * D3DX_PI - ab;
		}
		return ab;
		*/
	//}
	//else {
		//return 0.0f;
	//}
}

namespace math {
/**
/ will normalize t to 0-1 in the range of 0.0 - max
*/
float norm(float t,float max);

bool addClamped(float *value,float add,float max,float newValue = 0.0f);
float saturate(float x);
float smoothstep(float min,float max,float x);
float smoothstep(float x);
void clamp(int* value,int min,int max);
void clamp(float* value,float min,float max);
float clamp(float value,float min = 0.0f,float max = 1.0f);
Vec2 getRadialVelocity(float angle,float vel);
float getAngle(float x1,float y1,float x2,float y2);
float getAngle(const Vec2& v1,const Vec2& v2);
float getTargetAngle(const Vec2& v1,const Vec2& v2);
Vec2 getDistantPosition(float x,float y,float angle,float radius);
Vec2 getDistantPosition(const Vec2& initialPosition,float angle,float radius);
float impulse(float k,float x );
float cubicPulse( float c, float w, float x );
float almostIdentity( float x, float m, float n );
void transformMousePosToWorld(Vec3* vPickRayDir,Vec3* vPickRayOrig);
void getTextureCoordinates(const Rect& textureRect,int textureSize,float* u1,float* v1,float* u2,float* v2,bool useHalfTexel = true);
void follow(const Vec2& targetPos,Vec2& newPos,float* angle,float distance,float add);
void clipToRect(Vec2& newPos,const Rect& rect);
bool isOutside(Vec2& pos,const Rect& rect);
float reflect(float angle);
void interpolateColor(const Color& firstCol,const Color& secondColor,float t,Color* outColor);
// -------------------------------------------------------
// Checks if value is lower min or higher max
// -------------------------------------------------------
bool outside(float value,float min,float max);

}

struct TexCoord {

	float u1;
	float u2;
	float v1;
	float v2;

	TexCoord() {
		u1 = v1 = 0.0f;
		u2 = v2 = 1.0f;
	}	
	void scale(const Rect& rect,int size) {
		float dim = (float)size;
		u1 = static_cast<float>(rect.left) / static_cast<float>(dim);
		u2 = static_cast<float>(rect.right) / static_cast<float>(dim);
		v1 = static_cast<float>(rect.top) / static_cast<float>(dim);
		v2 = static_cast<float>(rect.bottom) / static_cast<float>(dim);
		math::getTextureCoordinates(rect,size,&u1,&v1,&u2,&v2,true);
	}
};

}
