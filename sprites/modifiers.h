#pragma once
#include <Vector.h>
#include "..\utils\Color.h"

namespace ds {

	namespace modifiers {

		void bounce(Vector2f* positions, Vector2f* velocities, int num, float left, float right, float bottom, float top);

		void tick(float* timers, int num, float dt);

		void dampVelocities(Vector2f* velocities, int num , float damping, float min);

		void move(Vector2f* positions, Vector2f* velocities, int num, float dt);

		void lerpScale(Vector2f* scales, float* timers, int num, const Vector2f& min, const Vector2f& max, float ttl);

		void lerpColors(Color* colors, float* timers, int num, const Color& min, const Color& max, float ttl);

		void moveTowards(Vector2f* positions, int num, const Vector2f& target, float velocity, float dt);

		void moveTowards(Vector2f* positions,float* angles, int num, const Vector2f& target, float velocity, float dt);

		void follow(Vector2f* positions, float* angles, int num, const Vector2f& target, float velocity, float dt);

		void followRelative(Vector2f* positions, float* angles, int num, const Vector2f& target, float minDistance,float percentage, float dt);

		void handleKeyboard(Vector2f* pos,float speed,float dt);
	}

}