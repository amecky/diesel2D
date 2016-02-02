#include "tweening.h"
#include "..\dxstdafx.h"
#include "..\sprites\SpriteBatch.h"

// The code is taking from here https://github.com/jesusgollonet/ofpennereasing/tree/master/PennerEasing
// Everything is based on http://robertpenner.com/easing/
// Visualisation can be found here http://easings.net/de

namespace tweening {

	float linear(float t, float start, float end, float duration) {
		return start + ( end - start ) * t / duration;
	}

	// -------------------------------------------------------
	// easeSine
	// -------------------------------------------------------
	float easeSinus(float t, float start, float end, float duration) {
		return start + (end - start) * sin(t / duration * PI);
	}

	// -------------------------------------------------------
	// easeInQuad
	// -------------------------------------------------------
	float easeInQuad(float t, float start, float end, float duration) {
		float c = end - start;
		return start + c * t / duration * t;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	float easeOutQuad(float t, float start, float end, float duration) {
		return start - (end - start) * t / duration * (t - 2.0f);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	float easeInOutQuad(float t, float start, float end, float duration) {
		float c = end - start;
		if ((t /= duration / 2) < 1) return ((c / 2)*(t*t)) + start;
		return -c / 2 * (((t - 2)*(--t)) - 1) + start;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	float easeInQuart(float start,float end,float t) {
		return start + (end - start) * pow (t, 4);
	}

	// -------------------------------------------------------
	// easeInBack
	// -------------------------------------------------------
	float easeInBack(float t, float start, float end, float duration) {
		float c = end - start;
		float s = 1.70158f;
		float postFix = t / duration;
		return c*(postFix)*t*((s + 1.0f)*t - s) + start;
	}

	// -------------------------------------------------------
	// easeOutBack
	// -------------------------------------------------------
	float easeOutBack(float t, float start, float end, float duration) {
		float c = end - start;
		float s = 1.70158f;
		return c*((t = t / duration - 1)*t*((s + 1.0f)*t + s) + 1.0f) + start;
	}

	// -------------------------------------------------------
	// easeInOutBack
	// -------------------------------------------------------
	float easeInOutBack(float t, float start, float end, float duration) {
		float c = end - start;
		float s = 1.70158f;
		if ((t /= duration / 2.0f) < 1.0f) {
			return c / 2.0f * (t*t*(((s *= (1.525f)) + 1.0f)*t - s)) + start;
		}
		float postFix = t -= 2;
		return c / 2.0f * ((postFix)*t*(((s *= (1.525f)) + 1.0f)*t + s) + 2.0f) + start;
	}

	float easeInCubic(float t, float start, float end, float duration) {
		float c = end - start;
		return c*(t /= duration)*t*t + start;
	}
	float easeOutCubic(float t, float start, float end, float duration) {
		float c = end - start;
		return c*((t = t / duration - 1)*t*t + 1) + start;
	}

	float easeInOutCubic(float t, float start, float end, float duration) {
		float c = end - start;
		if ((t /= duration / 2) < 1) return c / 2 * t*t*t + start;
		return c / 2 * ((t -= 2)*t*t + 2) + start;
	}

	float easeInElastic(float t, float start, float end, float duration) {
		float c = end - start;
		if (t == 0) return start;  
		if ((t /= duration) == 1) return start + c;
		float p = duration * 0.3f;
		float a = c;
		float s = p / 4;
		float postFix = a*pow(2, 10 * (t -= 1)); // this is a fix, again, with post-increment operators
		return -(postFix * sin((t*duration - s)*(2 * PI) / p)) + start;
	}

	float easeOutElastic(float t, float start, float end, float duration) {
		float c = end - start;
		if (t == 0) return start;  
		if ((t /= duration) == 1) return start + c;
		float p = duration * 0.3f;
		float a = c;
		float s = p / 4;
		return (a*pow(2, -10 * t) * sin((t*duration - s)*(2 * PI) / p) + c + start);
	}

	float easeInOutElastic(float t, float start, float end, float duration) {
		float c = end - start;
		if (t == 0) return start;  
		if ((t /= duration / 2) == 2) return start + c;
		float p = duration * (0.3f * 1.5f);
		float a = c;
		float s = p / 4;

		if (t < 1) {
			float postFix = a*pow(2, 10 * (t -= 1)); // postIncrement is evil
			return -.5f*(postFix* sin((t*duration - s)*(2 * PI) / p)) + start;
		}
		float postFix = a*pow(2, -10 * (t -= 1)); // postIncrement is evil
		return postFix * sin((t*duration - s)*(2 * PI) / p)*.5f + c + start;
	}
	// -------------------------------------------------------
	// interpolate
	// -------------------------------------------------------
	float interpolate(TweeningType type, float start, float end, float t, float duration) {
		return type(t,start,end,duration);
	}

	// -------------------------------------------------------
	// Vector2f interpolate
	// -------------------------------------------------------
	Vector2f interpolate(TweeningType type, const Vector2f& start, const Vector2f& end, float t, float duration) {
		return Vector2f(interpolate(type,start.x,end.x,t,duration),interpolate(type,start.y,end.y,t,duration));
	}

	// -------------------------------------------------------
	// Vector2f interpolate
	// -------------------------------------------------------
	ds::Color interpolate(TweeningType type, const ds::Color& start, const ds::Color& end, float t, float duration) {
		return ds::Color(
			interpolate(type, start.r, end.r, t, duration),
			interpolate(type, start.g, end.g, t, duration),
			interpolate(type, start.b, end.b, t, duration),
			interpolate(type, start.a, end.a, t, duration)
		);
	}

	TweeningType get_by_index(int index) {
		switch (index) {
			case 0: return linear; break;		
			case 1: return easeSinus; break;
			case 2: return easeInQuad; break;
			case 3: return easeOutQuad; break;
			case 4: return easeInOutQuad; break;
			case 5: return easeInBack; break;
			case 6: return easeOutBack; break;
			case 7: return easeInOutBack; break;
			case 8: return easeInCubic; break;
			case 9: return easeOutCubic; break;
			case 10: return easeInOutCubic; break;
			case 11: return easeInElastic; break;
			case 12: return easeOutElastic; break;
			case 13: return easeInOutElastic; break;
		}
		return linear;
	}

	void draw(TweeningType type, const ds::Texture& texture, float step, float delta) {
		v2 p(100, 100);
		float t = 0.0f;
		int num = 1.0f / step;
		for (int i = 0; i < num; ++i) {
			if (t <= delta) {
				float y = interpolate(type, 0.0f, 200.0f, t,1.0f);
				p.x = 100 + i * 10.0f;
				p.y = 100 + y;
				ds::sprites::draw(p, texture);
			}
			t += step;
		}
	}

}
