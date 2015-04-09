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

}
