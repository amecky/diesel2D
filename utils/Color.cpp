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
}

}