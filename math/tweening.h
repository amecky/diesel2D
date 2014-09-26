#pragma once
#include <Vector.h>

namespace tweening {

	typedef float (*TweeningType)(float,float,float);

	float linear(float start,float end,float t);

	float easeInQuad(float start,float end,float t);

	float easeOutQuad(float start,float end,float t);

	float easeInOutQuad(float start,float end,float t);

	float easeInQuart(float start,float end,float t);

	float interpolate(TweeningType type,float start,float end,float t);

	Vector2f interpolate(TweeningType type,const Vector2f& start,const Vector2f& end,float t);

};

