#include "tweening.h"
#include "..\dxstdafx.h"

namespace tweening {

	float linear(float start,float end,float t) {
		return start + ( end - start ) * t;
	}

	// -------------------------------------------------------
	// easeSine
	// -------------------------------------------------------
	float easeSinus(float start,float end,float t) {
		return start + (end - start) * sin(t * PI);
	}

	// -------------------------------------------------------
	// easeInQuad
	// -------------------------------------------------------
	float easeInQuad(float start,float end,float t) {
		return start + (end - start) * t * t;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	float easeOutQuad(float start,float end,float t) {
		return start - (end - start) * t * (t - 2.0f);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	float easeInOutQuad(float start,float end,float t) {
		float tt = t * 2.0f;
		if ( tt < 1.0f) {
			return start + (end-start) * 0.5f * tt * tt;
		}
		return start - (end - start) / 2.0f * (((tt-2.0f)*(--tt)) - 1.0f);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	float easeInQuart(float start,float end,float t) {
		return start + (end - start) * pow (t, 4);
	}

	float easeInBack(float start, float end, float t) {
		float s = 1.70158f;
		return (end - start) * t * ((s + 1.0f) * t - s) + start;
	}
	float easeOutBack(float start, float end, float t) {
		float s = 1.70158f;
		return (end - start) * ((t - 1.0f) * t * ((s + 1.0f) * t + s) + 1.0f) + start;
	}

	float easeInOutBack(float start, float end, float t) {
		float s = 1.70158f;
		if ( t < 0.5f ) {
			return (end - start) / 2.0f * (t * t * (((s *= (1.525f)) + 1.0f) * t - s)) + start;
		}
		float postFix = t -= 2;
		return (end - start) / 2.0f * ((postFix) * t * (((s *= (1.525f)) + 1) * t + s) + 2.0f) + start;
	}

	// -------------------------------------------------------
	// interpolate
	// -------------------------------------------------------
	float interpolate(TweeningType type,float start,float end,float t) {
		return type(start,end,t);
	}

	// -------------------------------------------------------
	// Vector2f interpolate
	// -------------------------------------------------------
	Vector2f interpolate(TweeningType type,const Vector2f& start,const Vector2f& end,float t) {
		return Vector2f(interpolate(type,start.x,end.x,t),interpolate(type,start.y,end.y,t));
	}

	// -------------------------------------------------------
	// Vector2f interpolate
	// -------------------------------------------------------
	ds::Color interpolate(TweeningType type, const ds::Color& start, const ds::Color& end, float t) {
		return ds::Color(
			interpolate(type, start.r, end.r, t), 
			interpolate(type, start.g, end.g, t),
			interpolate(type, start.b, end.b, t),
			interpolate(type, start.a, end.a, t)
		);
	}

}
