#include "modifiers.h"
#include "..\math\vector.h"

namespace ds {

	namespace modifiers {

		// -------------------------------------------------------
		// bounce
		// -------------------------------------------------------
		void bounce(Vector2f* positions, Vector2f* velocities, int num, float left, float right, float bottom, float top) {
			Vector2f* p = positions;
			for (int i = 0; i < num; ++i) {
				if (p[i].x < left || p[i].x > right) {
					velocities[i].x *= -1.0f;
				}
				if (p[i].y < bottom || p[i].y > top) {
					velocities[i].y *= -1.0f;
				}
			}
		}

		void tick(float* timers, int num, float dt) {
			for (int i = 0; i < num; ++i) {
				timers[i] += dt;
			}
		}

		// -------------------------------------------------------
		// Damp velocities
		// -------------------------------------------------------
		void dampVelocities(Vector2f* velocities, int num , float damping, float min) {
			Vector2f* v = velocities;
			for (int i = 0; i < num; ++i) {
				float l = length(*v);
				if (l > min) {
					*v *= damping;
				}
				++v;
			}
		}

		// -------------------------------------------------------
		// Move
		// -------------------------------------------------------
		void move(Vector2f* positions, Vector2f* velocities, int num, float dt) {
			Vector2f* p = positions;
			Vector2f* v = velocities;
			for (int i = 0; i < num; ++i) {
				*p += *v * dt;
				++p;
				++v;
			}

		}

		// -------------------------------------------------------
		// Move torwards target with constant speed
		// -------------------------------------------------------
		void moveTowards(Vector2f* positions, int num, const Vector2f& target, float velocity, float dt) {
			for (int i = 0; i < num; ++i) {
				Vector2f diff = target - positions[i];
				Vector2f n = normalize(diff);
				n *= velocity * dt;
				positions[i] += n;
			}
		}

		// -------------------------------------------------------
		// Move torwards target with constant speed
		// -------------------------------------------------------
		void moveTowards(Vector2f* positions, float* angles, int num, const Vector2f& target, float velocity, float dt) {
			for (int i = 0; i < num; ++i) {
				Vector2f diff = target - positions[i];
				Vector2f n = normalize(diff);
				n *= velocity * dt;
				positions[i] += n;
				angles[i] = vector::calculateRotation(n);
			}
		}

		// -------------------------------------------------------
		// lerp scale
		// -------------------------------------------------------
		void lerpScale(Vector2f* scales, float* timers, int num, const Vector2f& min, const Vector2f& max, float ttl) {
			for (int i = 0; i < num; ++i) {
				float norm = timers[i] / ttl;
				if (norm > 1.0f) {
					norm = 1.0f;
				}
				if (norm < 0.0f) {
					norm = 0.0f;
				}
				scales[i] = min * (1.0f - norm) + max * norm;
			}
		}

		// -------------------------------------------------------
		// lerp colors
		// -------------------------------------------------------
		void lerpColors(Color* colors, float* timers, int num, const Color& min, const Color& max, float ttl) {
			for (int i = 0; i < num; ++i) {
				float norm = timers[i] / ttl;
				if (norm > 1.0f) {
					norm = 1.0f;
				}
				if (norm < 0.0f) {
					norm = 0.0f;
				}
				colors[i] = color::lerp(min, max, norm);
			}
		}

		// -------------------------------------------------------
		// follow with constant speed
		// -------------------------------------------------------
		void follow(Vector2f* positions, float* angles, int num, const Vector2f& target, float velocity, float dt) {
			if (num > 0) {
				moveTowards(&positions[0], &angles[0], 1, target, velocity, dt);
			}
			if (num > 1) {
				for (int i = 1; i < num; ++i) {
					moveTowards(&positions[i], &angles[i], 1, positions[i-1], velocity, dt);
				}
			}
		}

		// -------------------------------------------------------
		// follow relative and only setting positions
		// -------------------------------------------------------
		void followRelative(const Vector2f& targetPos, Vector2f& newPos, float* angle, float minDistance, float percentage) {
			Vector2f p = newPos;
			Vector2f diff = targetPos - p;
			*angle = vector::calculateRotation(diff);
			float dist = distance(targetPos, p);
			if (dist > minDistance) {
				newPos.x = newPos.x + diff.x * percentage;
				newPos.y = newPos.y + diff.y * percentage;
			}
		}

		// -------------------------------------------------------
		// follow relative and set positions and angles
		// -------------------------------------------------------
		void followRelative(Vector2f* positions, float* angles, int num, const Vector2f& target, float minDistance, float percentage, float dt) {
			if (num > 0) {
				followRelative(target, positions[0], &angles[0], minDistance, percentage);
				
			}
			if (num > 1) {
				for (int i = 1; i < num; ++i) {
					followRelative(positions[i - 1], positions[i], &angles[i], minDistance, percentage);
				}
			}
		}

		// -------------------------------------------------------
		// WASD keyboard controller
		// -------------------------------------------------------
		void handleKeyboard(Vector2f* pos, float speed, float dt) {
			Vector2f move;
			if (GetAsyncKeyState('W') & 0x8000) {
				move.y += speed * dt;
			}
			if (GetAsyncKeyState('S') & 0x8000) {
				move.y -= speed * dt;
			}
			if (GetAsyncKeyState('A') & 0x8000) {
				move.x -= speed * dt;
			}
			if (GetAsyncKeyState('D') & 0x8000) {
				move.x += speed * dt;
			}
			*pos += move;
		}
	}

}