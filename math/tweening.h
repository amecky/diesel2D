#pragma once
#include <Vector.h>
#include "..\utils\Color.h"

// http://gizma.com/easing/
namespace tweening {

	typedef float (*TweeningType)(float,float,float);

	float linear(float start,float end,float t);

	float easeInQuad(float start,float end,float t);

	float easeSinus(float start,float end,float t);

	float easeOutQuad(float start,float end,float t);

	float easeInOutQuad(float start,float end,float t);

	float easeInQuart(float start,float end,float t);

	float easeInBack(float start, float end, float t);

	float easeOutBack(float start, float end, float t);

	float easeInOutBack(float start, float end, float t);

	float interpolate(TweeningType type,float start,float end,float t);

	Vector2f interpolate(TweeningType type,const Vector2f& start,const Vector2f& end,float t);

	ds::Color interpolate(TweeningType type, const ds::Color& start, const ds::Color& end, float t);

};

