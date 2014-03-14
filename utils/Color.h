#pragma once
#include "..\dxstdafx.h"
#include <iostream>

namespace ds {

struct Color : public D3DXCOLOR {

public:
	Color() : D3DXCOLOR() {}
	Color(int r,int g,int b,int a);
	Color(uchar r,uchar g,uchar b,uchar a);
	Color(int r,int g,int b);
	Color(float rc,float gc,float bc);
	Color(float rc,float gc,float bc,float ac);
	Color(const D3DCOLORVALUE& v);
	// static constants
	static const Color WHITE;
	static const Color BLACK;
	static const Color CYAN;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
	static const Color GRAY40;
	static const Color GRAY25;
	static const Color GRAY65;

	void operator = (const D3DXCOLOR& v) {
		r = v.r;
		g = v.g;
		b = v.b;
		a = v.a;
	}
};

inline Color::Color(const D3DCOLORVALUE& v) {
	r = v.r;
	g = v.g;
	b = v.b;
	a = v.a;
}

inline Color::Color(int rc,int gc,int bc,int ac) {
	r = (float)rc / 255.0f;
	g = (float)gc / 255.0f;
	b = (float)bc / 255.0f;
	a = (float)ac / 255.0f;
}

inline Color::Color(uchar rc,uchar gc,uchar bc,uchar ac) {
	r = (float)rc / 255.0f;
	g = (float)gc / 255.0f;
	b = (float)bc / 255.0f;
	a = (float)ac / 255.0f;
}

inline Color::Color(int rc,int gc,int bc) {
	r = (float)rc / 255.0f;
	g = (float)gc / 255.0f;
	b = (float)bc / 255.0f;
	a = 1.0f;
}

inline Color::Color(float rc,float gc,float bc) {
	r = rc;
	g = gc;
	b = bc;
	a = 1.0f;
}

inline Color::Color(float rc,float gc,float bc,float ac) {
	r = rc;
	g = gc;
	b = bc;
	a = ac;
}

inline std::ostream& operator << (std::ostream& os, const Color& c) {
	os << c.r;
	os << c.g;
	os << c.b;
	os << c.a;
	return os;
}

inline std::istream& operator >> (std::istream& input, Color& c) {
	input >> c.r;
	//c.r /= 255.0f;
	input >> c.g;
	//c.g /= 255.0f;
	input >> c.b;
	//c.b /= 255.0f;
	input >> c.a;
	//c.a /= 255.0f;
	return input;
}


namespace color {

	Color lerp(const Color& lhs,const Color& rhs,float t);

	Color hsvToColor(float h,float s,float v);

}

//ds::Color operator *= (ds::Color& u,const ds::Color& other);
//ds::Color operator += (ds::Color& u,ds::Color other);
//ds::Color operator + (const ds::Color& u,const ds::Color& other);
//ds::Color operator * (const ds::Color& u,const ds::Color& other);
//ds::Color operator * (const ds::Color& u,float v);

};

ds::Color catmullRom(float t,const ds::Color& v0,const ds::Color& v1,const ds::Color& v2,const ds::Color& v3);


