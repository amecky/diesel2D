#pragma once
#include <iostream>

namespace ds {

const float PI = 3.141592654f;

// -------------------------------------------------------
// Vec2
// -------------------------------------------------------
struct Vec2 {

	float x,y;
	Vec2();
	Vec2(float _x,float _y);
	Vec2(int _x,int _y);
	Vec2(const Vec2& v);

	Vec2& operator += (const Vec2& v) {
		Vec2 u;
		x += v.x;
		y += v.y;
		return *this;
	}
	Vec2& operator -() {
		x *= -1.0f;
		y *= -1.0f;
		return *this;
	}
	operator float *() const { return (float *) &x; }
};

inline Vec2::Vec2() : x(0.0f) , y(0.0f) {}

inline Vec2::Vec2(float _x,float _y) : x(_x) , y(_y) {}

inline Vec2::Vec2(const Vec2& v) : x(v.x) , y(v.y) {}

inline Vec2::Vec2(int _x,int _y) {
	x = static_cast<float>(_x);
	y = static_cast<float>(_y);
}

inline std::ostream& operator << (std::ostream& os, const Vec2& v) {
	os << v.x;
	os << v.y;
	return os;
}

inline std::istream& operator >> (std::istream& input, Vec2& v) {
	input >> v.x;
	input >> v.y;	
	return input;
}

const Vec2 V2_UP    = Vec2(0.0f,-1.0f);
const Vec2 V2_RIGHT = Vec2(1.0f,0.0f);
const Vec2 V2_LEFT  = Vec2(-1.0f,0.0f);
const Vec2 V2_DOWN  = Vec2(0.0f,1.0f);
// -------------------------------------------------------
// Vec3
// -------------------------------------------------------
struct Vec3 {

	float x,y,z;

	Vec3() : x(0.0f) , y(0.0f) , z(0.0f) {}
	Vec3(float _x,float _y,float _z) : x(_x) , y(_y) , z(_z) {}

	Vec2 xy() {
		return Vec2(x,y);
	}

	operator float *() const { return (float *) &x; }
};



// -------------------------------------------------------
// Vec4
// -------------------------------------------------------
struct Vec4 {

	float x,y,z,w;

	Vec4() : x(0.0f) , y(0.0f) , z(0.0f) , w(0.0f) {}
	Vec4(float _x,float _y,float _z,float _w) : x(_x) , y(_y) , z(_z) , w(_w) {}

	operator float *() const { return (float *) &x; }

};

// -------------------------------------------------------
// 3x3 Matrix
// -------------------------------------------------------
struct mat3 {

	union {
		struct {
			float        _11, _12, _13;
			float        _21, _22, _23;
			float        _31, _32, _33;
		};
		float m[3][3];
	};

	//Vec3 a,b,c;
	mat3();

	mat3(float m11,float m12,float m13,float m21,float m22,float m23,float m31,float m32,float m33);

	operator float *() const { return (float *) &_11; }
};

inline mat3::mat3() {
	for ( int i = 0; i < 3; ++i ) {
		for ( int j = 0; j < 3; ++j ) {
			m[i][j] = 0.0f;
		}
	}
}


inline mat3::mat3(float m11,float m12,float m13,float m21,float m22,float m23,float m31,float m32,float m33) {
	_11 = m11;
	_12 = m12;
	_13 = m13;
	_21 = m21;
	_22 = m22;
	_23 = m23;
	_31 = m31;
	_32 = m32;
	_33 = m33;	
}

// -------------------------------------------------------
// 4x4 Matrix
// -------------------------------------------------------
struct mat4 {

	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};

	mat4();
	mat4(float m11,float m12,float m13,float m14,float m21,float m22,float m23,float m24,float m31,float m32,float m33,float m34,float m41,float m42,float m43,float m44);

	operator float *() const { return (float *) &_11; }
};

inline mat4::mat4() {
	for ( int i = 0; i < 4; ++i ) {
		for ( int j = 0; j < 4; ++j ) {
			m[i][j] = 0.0f;
		}
	}
}

inline mat4::mat4(float m11,float m12,float m13,float m14,float m21,float m22,float m23,float m24,float m31,float m32,float m33,float m34,float m41,float m42,float m43,float m44) {
	_11 = m11;
	_12 = m12;
	_13 = m13;
	_14 = m14;
	_21 = m21;
	_22 = m22;
	_23 = m23;
	_24 = m24;
	_31 = m31;
	_32 = m32;
	_33 = m33;
	_34 = m34;
	_41 = m41;
	_42 = m42;
	_43 = m43;
	_44 = m44;
}

// -------------------------------------------------------
// Rect
// -------------------------------------------------------
struct Rect {
	
	float top,left,bottom,right;
	Rect();
	Rect(const Rect& rect);
	Rect(float top,float left,float width,float height);
	Rect(int _top,int _left,int _width,int _height);
	const float width() const;
	const float height() const;
};

inline Rect::Rect() : top(0.0f) , left(0.0f) , bottom(0.0f) , right(0.0f) {}

inline Rect::Rect(float _top,float _left,float width,float height) : top(_top) , left(_left) {
	bottom = _top + height;
	right = _left + width;
}

inline Rect::Rect(int _top,int _left,int _width,int _height) {
	top = static_cast<float>(_top);
	left = static_cast<float>(_left);
	bottom = top + static_cast<float>(_height);
	right = left + static_cast<float>(_width);
}

inline Rect::Rect(const Rect& rect) {
	top = rect.top;
	left = rect.left;
	bottom = rect.bottom;
	right = rect.right;
}

inline const float Rect::width() const {
	return right - left;
}

inline const float Rect::height() const {
	return bottom - top;
}

inline std::ostream& operator << (std::ostream& os, const Rect& rect) {
	os << rect.top;
	os << rect.left;
	os << rect.width();
	os << rect.height();
	return os;
}

inline std::istream& operator >> (std::istream& input, Rect& rect) {
	input >> rect.top;
	input >> rect.left;
	float w;
	input >> w;
	rect.right = rect.left + w;
	float h;
	input >> h;
	rect.bottom = rect.top + h;
	return input;
}

// -------------------------------------------------------
// Circle
// -------------------------------------------------------
struct Circle {
	
	Vec2 pos;
	float radius;

	Circle() : pos(0.0f,0.0f) , radius(0.0f) {}

	Circle(const Vec2& p,float r) : pos(p) , radius(r) {}
};

// -------------------------------------------------------
// Point
// -------------------------------------------------------
struct Point {

	float x,y;

	Point() : x(0.0f) , y(0.0f) {}

	Point(float xp,float yp) : x(xp) , y(yp) {}
};

}