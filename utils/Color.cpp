#include "Color.h"

namespace ds {

const Color Color::WHITE( 1.0f, 1.0f, 1.0f,1.0f);
const Color Color::BLACK( 0.0f, 0.0f, 0.0f,1.0f );
const Color Color::CYAN( 0.0f, 1.0f, 1.0f,1.0f );	
const Color Color::RED( 1.0f, 0.0f, 0.0f, 1.0f );
const Color Color::GREEN( 0.0f, 1.0f, 0.0f, 1.0f );
const Color Color::BLUE( 0.0f, 0.0f, 1.0f, 1.0f );
const Color Color::YELLOW( 1.0f, 1.0f, 0.0f, 1.0f );	
const Color Color::GRAY40( 0.4f, 0.4f, 0.4f, 1.0f );
const Color Color::GRAY25( 0.25f, 0.25f, 0.25f, 1.0f);
const Color Color::GRAY65( 0.65f, 0.65f, 0.65f, 1.0f);

namespace color {

	Color lerp(const Color& lhs,const Color& rhs,float t) {
		if ( t <= 0.0f ) {
			return lhs;
		}
		if ( t >= 1.0f ) {
			return rhs;
		}
		float invT = 1.0f - t;
		return Color(lhs.r * invT + rhs.r * t,lhs.g * invT + rhs.g * t,lhs.b * invT + rhs.b * t,lhs.a * invT + rhs.a * t);
	}

	// 0 < h < 360 / 0 < s < 100 / 0 < v < 100
	Color hsvToColor(float h,float s,float v) {
		if (h == 0 && s == 0) {
			return Color(v / 100.0f, v / 100.0f, v / 100.0f);
		}
		
		float hh = h / 60.0f;

		float c = s / 100.0f * v / 100.0f;
		float x = c * (1.0f - abs(fmod(hh,2.0f) - 1.0f));
		Color clr;
		if (hh < 1.0f) {
			clr = Color(c, x, 0.0f);
		}
		else if (hh < 2.0f) {
			clr = Color(x, c, 0.0f);
		}
		else if (hh < 3.0f) {
			clr = Color(0.0f, c, x);
		}
		else if (hh < 4.0f) {
			clr = Color(0.0f, x, c);
		}
		else if (hh < 5) {
			clr = Color(x, 0.0f, c);
		}
		else {
			clr = Color(c, 0.0f, x);
		}
		float m = v / 100.0f - c;
		clr.r += m;
		clr.g += m;
		clr.b += m;
		return clr;
	}
}

}

ds::Color catmullRom(float t,const ds::Color& v0,const ds::Color& v1,const ds::Color& v2,const ds::Color& v3) {
	return v0;
}
/*
ds::Color operator *= (ds::Color& u,const ds::Color& other) {
	u.r *= other.r;
	u.g *= other.g;
	u.b *= other.b;
	u.a *= other.a;
	return u;
}

ds::Color operator * (const ds::Color& u,const ds::Color& other) {
	ds::Color ret = u;
	ret *= other;
	return ret;
}

ds::Color operator * (const ds::Color& u,float v) {
	ds::Color ret = u;
	ret.r *= v;
	ret.g *= v;
	ret.b *= v;
	ret.a *= v;
	return ret;
}

ds::Color operator += (ds::Color& u,ds::Color other) {
	u.r += other.r;
	u.g += other.g;
	u.b += other.b;
	u.a += other.a;
	return u;
}

ds::Color operator + (const ds::Color& u,const ds::Color& other) {
	ds::Color ret = u;
	ret += other;
	return ret;
}
*/