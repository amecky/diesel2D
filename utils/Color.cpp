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

	Color hsvToColor(float h,float s,float v) {
		if (h == 0 && s == 0) {
			return Color(v, v, v);
		}
		float c = s * v;
		float x = c * (1.0f - abs(fmod(h,2.0f) - 1.0f));
		float m = v - c;

		if (h < 1) return Color(c + m, x + m, m);
		else if (h < 2) return Color(x + m, c + m, m);
		else if (h < 3) return Color(m, c + m, x + m);
		else if (h < 4) return Color(m, x + m, c + m);
		else if (h < 5) return Color(x + m, m, c + m);
		else return Color(c + m, m, x + m);
	}
}

}