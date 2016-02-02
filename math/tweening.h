#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\renderer\render_types.h"

// http://gizma.com/easing/
namespace tweening {

	typedef float (*TweeningType)(float,float,float,float);

	float linear(float t,float start,float end,float duration);

	float easeInQuad(float t, float start, float end, float duration);

	float easeSinus(float t, float start, float end, float duration);

	float easeOutQuad(float t, float start, float end, float duration);

	float easeInOutQuad(float t, float start, float end, float duration);

	float easeInQuart(float start,float end,float t);

	float easeInBack(float t, float start, float end, float duration);

	float easeOutBack(float t, float start, float end, float duration);

	float easeInOutBack(float t, float start, float end, float duration);

	float easeInCubic(float t, float start, float end, float duration);

	float easeOutCubic(float t, float start, float end, float duration);

	float easeInOutCubic(float t, float start, float end, float duration);

	float easeInElastic(float t, float start, float end, float duration);

	float easeOutElastic(float t, float start, float end, float duration);

	float easeInOutElastic(float t, float start, float end, float duration);

	TweeningType get_by_index(int index);

	float interpolate(TweeningType type, float start, float end, float t, float duration);

	Vector2f interpolate(TweeningType type,const Vector2f& start,const Vector2f& end,float t,float duration);

	ds::Color interpolate(TweeningType type, const ds::Color& start, const ds::Color& end, float t, float duration);

	void draw(TweeningType type, const ds::Texture& texture,float step = 0.1f, float delta = 1.0f);

};

