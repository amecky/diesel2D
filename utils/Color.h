#pragma once
#include "..\dxstdafx.h"

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


namespace color {

	Color lerp(const Color& lhs,const Color& rhs,float t);

}

};


