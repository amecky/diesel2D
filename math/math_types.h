#pragma once

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
};

inline Vec2::Vec2() : x(0.0f) , y(0.0f) {}

inline Vec2::Vec2(float _x,float _y) : x(_x) , y(_y) {}

inline Vec2::Vec2(const Vec2& v) : x(v.x) , y(v.y) {}

inline Vec2::Vec2(int _x,int _y) {
	x = static_cast<float>(_x);
	y = static_cast<float>(_y);
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
	Vec3();

	Vec3(float _x,float _y,float _z);
};

inline Vec3::Vec3() : x(0.0f) , y(0.0f) , z(0.0f) {}

inline Vec3::Vec3(float _x,float _y,float _z) : x(_x) , y(_y) , z(_z) {}

// -------------------------------------------------------
// 3x3 Matrix
// -------------------------------------------------------
struct mat3 {
	Vec3 a,b,c;
	mat3();
};

inline mat3::mat3() : a() , b(), c() {}

// -------------------------------------------------------
// Rect
// -------------------------------------------------------
struct Rect {
	
	float top,left,bottom,right;
	Rect();
	Rect(const Rect& rect);
	Rect(float top,float left,float width,float height);
	const float width() const;
	const float height() const;
};

inline Rect::Rect() : top(0.0f) , left(0.0f) , bottom(0.0f) , right(0.0f) {}

inline Rect::Rect(float _top,float _left,float width,float height) : top(_top) , left(_left) {
	bottom = _top + height;
	right = _left + width;
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

}