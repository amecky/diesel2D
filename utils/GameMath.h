#pragma once
#include "..\dxstdafx.h"
#include "Color.h"

namespace ds {

class Rect : public RECT {

public:
	Rect() { 
		top = left = bottom = right = 0;
	}
	Rect(int _top,int _left,int _width,int _height) { 
		top = _top;
		left = _left;
		bottom = top+_height;
		right = left+_width;
	}
	Rect(uint32 _top,uint32 _left,uint32 _width,uint32 _height) { 
		top = _top;
		left = _left;
		bottom = top+_height;
		right = left+_width;
	}
	Rect(float _top,float _left,float _width,float _height) { 
		top = (LONG)_top;
		left = (LONG)_left;
		bottom = top+(LONG)_height;
		right = left+(LONG)_width;
	}
	Rect(RECT rct) { 
		top = rct.top ; 
		left = rct.left; 
		bottom = rct.bottom;
		right=rct.right;
	}
	int width() const {
		return right - left;
	}
	int height() const {
		return bottom - top;
	}
	void set(float _top,float _left,float _right,float _bottom) {
		top = (LONG)_top;
		bottom = (LONG)_bottom;
		left = (LONG)_left;
		right = (LONG)_right;
	}
};

class Vec3 : public D3DXVECTOR3 {

public:
	inline float Length() { return D3DXVec3Length(this); }
	inline Vec3 *Normalize() { return static_cast<Vec3 *>(D3DXVec3Normalize(this, this)); }
	inline float Dot(const Vec3 &b) { return D3DXVec3Dot(this, &b); }
	inline Vec3 Cross(const Vec3 &b) const;	
	Vec3(float p) { x = p; y = p; z = p;}
	Vec3(D3DXVECTOR3 &v3) { x = v3.x; y = v3.y; z = v3.z; }
	Vec3() : D3DXVECTOR3() { }
	Vec3(float _x, float _y, float _z) { x=_x; y=_y; z=_z; }
	Vec3(int _x, int _y, int _z) { x=(float)_x; y=(float)_y; z=(float)_z; }
	inline Vec3(const class Vec4 &v4);
	Vec3 operator+ (float value) { return Vec3(x += value,y += value,z+=value);  }
	Vec3 operator+ (const Vec3& v) { return Vec3(x += v.x,y += v.y,z+=v.z);  }
	void rotateZ(float degrees) {
		float nx = x * cos(DEGTORAD(degrees)) - y * sin(DEGTORAD(degrees));
		float ny = x * sin(DEGTORAD(degrees)) + y * cos(DEGTORAD(degrees));
		x = nx;
		y = ny;
	}
	void addScale(const Vec3& v,float scale) {
		x += v.x * scale;
		y += v.y * scale;
		z += v.z * scale;
	}	
	static const Vec3 ZERO;
	//operator float *() const { return (float *) &x; }
};
inline Vec3 Vec3::Cross(const Vec3 &b) const
{
	Vec3 out;
	D3DXVec3Cross(&out, this, &b);
	return out;
}

class Vec2 : public D3DXVECTOR2 {

public:	
	Vec2() : D3DXVECTOR2() { }
	Vec2(float _x, float _y) { 
		x=_x; 
		y=_y;
	}
	Vec2(int _x, int _y) { 
		x=(float)_x; 
		y=(float)_y;
	}
	Vec2(float p) { 
		x = p; 
		y = p; 
	}
	Vec2(D3DXVECTOR3 &v3) { 
		x = v3.x; 
		y = v3.y; 
	}
	Vec2(Vec3 &v3) { 
		x = v3.x; 
		y = v3.y; 
	}		
	Vec2 operator / (float v)  const {
		return Vec2(x / v,y / v);
	}
	/*
	Vec2 operator * (float v)  const {
		return Vec2(x * v,y * v);
	}
	*/
	Vec2& operator += (const Vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	Vec2& operator -= (const Vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	Vec2& operator *= (float v) {
		x *= v;
		y *= v;
		return *this;
	}
	Vec2& operator *= (const Vec2& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	Vec2& operator /= (float v) {
		x /= v;
		y /= v;
		return *this;
	}
	void add(float v) {
		x += v;
		y += v;
	}
	void add(float xv,float yv) {
		x += xv;
		y += yv;
	}
	void addRadial(float radius,float angle) {
		x += radius * cos(angle);
		y += radius * sin(angle);
	}
	void sub(float xv,float yv) {
		x -= xv;
		y -= yv;
	}
	void addScale(const Vec2& v,float scale) {
		x += v.x * scale;
		y += v.y * scale;
	}		
	Vec2 Cross(const Vec2& u) const {
		return Vec2(x * u.x,y*u.y);
	}

	bool operator == (const Vec2& u) {
		return x == u.x && y == u.y;
	}
	bool operator != (const Vec2& u) {
		return x != u.x || y != u.y;
	}	
	float Length() const { 
		return D3DXVec2Length(this); 
	}
	float SqrLength() {
		return x * x + y * y;
	}
	// returns the dot product
	float Dot(const Vec2 &b) const { 
		return D3DXVec2Dot(this, &b); 
	}
	// returns a normalized version of this vector
	Vec2 unit() const { 	
		float invLen = 1.0f / sqrtf(x * x + y *y );
		return Vec2(x * invLen,y*invLen);
	}
	// returns the length of the vector
	float distance(const Vec2& v) const {
		float dx = v.x - x;
		float dy = v.y - y;
		return sqrtf(dx*dx + dy*dy);
	}
	bool equals(const Vec2& v) {
		return x == v.x && y == v.y;
	}
	void rotate(float angle) {		
		float tmpx = x * cos(angle) - y * sin(angle);
		float tmpy = x * sin(angle) + y * cos(angle);
		x = tmpx;
		y = tmpy;
	}
	float getAngle(const Vec2& v2) const {	
		Vec2 vn1 = Vec2(x,y).unit();
		Vec2 vn2 = v2.unit();
		if ( vn1 != vn2 ) {			
			float dot = vn1.Dot(vn2);		
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
		}
		else {
			return 0.0f;
		}
	}
	void setScaledRadialVector(float angleDegrees,float velocity) {
		x = (float)cos(DEGTORAD(angleDegrees))*velocity;
		y = (float)sin(DEGTORAD(angleDegrees))*velocity;		
	}
	operator float *() const { return (float *) &x; }
	// static vectors
	static const Vec2 RIGHT;
	static const Vec2 ZERO;
	static const Vec2 UP;
	static const Vec2 DOWN;
	static const Vec2 LEFT;
};

Vec2 operator + (const Vec2& v1,const Vec2& v2);
Vec2 operator + (const Vec2 &v, const float s);
Vec2 operator + (const float s, const Vec2 &v);
Vec2 operator - (const Vec2& v1,const Vec2& v2);
Vec2 operator - (const Vec2 &v, const float s);
Vec2 operator - (const float s, const Vec2 &v);
/*
Vec2 operator * (const Vec2 &u, const Vec2 &v);
*/
Vec2 operator * (const float s, const Vec2 &v);
Vec2 operator * (const Vec2 &v, const float s);
/*
Vec2 operator / (const Vec2 &u, const Vec2 &v);
Vec2 operator / (const Vec2 &v, const float s);
Vec2 operator / (const float s, const Vec2 &v);
*/
bool operator == (const Vec2 &u, const Vec2 &v);
bool operator != (const Vec2 &u, const Vec2 &v);

class Matrix : public D3DXMATRIX {

public:
	Matrix() : D3DXMATRIX() {}
	inline void setPosition(const Vec3& pos);
	inline Vec3 getPosition() const;
	inline Matrix Inverse() const;
};

inline void Matrix::setPosition(const Vec3& pos) {
	m[3][0] = pos.x;
	m[3][1] = pos.y;
	m[3][2] = pos.z;
	m[3][3] = 1.0f;
}

inline Vec3 Matrix::getPosition() const {
	return Vec3(m[3][0], m[3][1], m[3][2]);
}

inline Matrix Matrix::Inverse() const {
	Matrix out;
	D3DXMatrixInverse(&out, NULL, this);
	return out;
}

inline Matrix operator * (const Matrix &a, const Matrix &b) {
	Matrix out;
	D3DXMatrixMultiply(&out, &a, &b);
	return out;
}

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
	if ( v1 != v2 ) {
		Vec2 vn1 = v1.unit();
		Vec2 vn2 = v2.unit();
		float dot = vn1.Dot(vn2);		
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
	}
	else {
		return 0.0f;
	}
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
