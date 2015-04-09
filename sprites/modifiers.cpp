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

		void bounce(DataContainer& container,int posIdx,int velIdx,int radiusIdx,float left, float right, float bottom, float top) {
			Vector2f* p = container.getChannel<Vector2f>(posIdx);
			Vector2f* v = container.getChannel<Vector2f>(velIdx);
			float* r = container.getChannel<float>(radiusIdx);
			for (int i = 0; i < container.size(); ++i) {
				if (p->x < ( left + *r ) || p->x > ( right - *r )) {
					v->x *= -1.0f;
				}
				if (p->y < ( bottom + *r) || p->y > ( top - *r )) {
					v->y *= -1.0f;
				}
				// FIXME: if velocity has changed push back 10%
				++p;
				++v;
				++r;
			}
		}

		void tick(float* timers, int num, float dt) {
			for (int i = 0; i < num; ++i) {
				timers[i] += dt;
			}
		}

		void tick(DataContainer& container,int timerIndex, float dt) {
			float* timers = container.getChannel<float>(timerIndex);
			for (uint32 i = 0; i < container.size(); ++i) {
				*timers += dt;
				++timers;
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

		void move(DataContainer& container,int posIdx,int velIdx,float dt) {
			Vector2f* position = container.getChannel<Vector2f>(posIdx);
			Vector2f* velocity = container.getChannel<Vector2f>(velIdx);
			for ( int i = 0; i < container.size();++i) {
				*position += *velocity * dt;
				++position;
				++velocity;
			}
		}

		// -------------------------------------------------------
		// rotate
		// -------------------------------------------------------
		void rotate(float* angles, int num, float dt) {
			for (int i = 0; i < num; ++i) {
				angles[i] += dt;
			}
		}

		void rotate(DataContainer& container, int anlgeIndex, float dt) {
			float* angles = container.getChannel<float>(anlgeIndex);
			for (uint32 i = 0; i < container.size(); ++i) {
				*angles += dt;
				++angles;
			}
		}

		float rotateTo(const Vector2f& position,const Vector2f& target) {
			float angle = 0.0f;
			Vector2f diff = target - position;
			Vector2f n = normalize(diff);			
			angle = vector::calculateRotation(n);
			return angle;
		}
		// -------------------------------------------------------
		// Move towards target with constant speed
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

		void flashColors(Color* colors, float* timers, int num, const Color& startColor, const Color& endColor, float amplitude) {
			for (int i = 0; i < num; ++i) {				
				float s = abs(sin(timers[i] * TWO_PI * amplitude));
				colors[i] = color::lerp(startColor, endColor, s);
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
		void handleKeyboard(Vector2f* pos,float* angle, float speed, float dt) {
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
			if (sqr_length(move) > 0.0f) {				
				*pos += move;
				*angle = vector::calculateRotation(move);
			}
		}

		// -------------------------------------------------------
		// separate
		// -------------------------------------------------------
		void separate(Vector2f* positions,int num,float radius,float relaxation) {
			for (int i = 0; i < num; ++i) {
				int idx = findNearest(positions[i], radius, positions,num, i);
				if (idx != -1) {
					Vector2f diff = positions[i] - positions[idx];
					Vector2f n = normalize(diff);
					n *= relaxation;
					positions[i] += n;
					positions[idx] -= n;
				}
			}
		}

		// -------------------------------------------------------
		// separate
		// -------------------------------------------------------
		void separate(Vector2f* positions, Vector2f* velocities,int num, float radius,float dt) {
			for (int i = 0; i < num; ++i) {
				int idx = findNearest(positions[i], radius, positions, num, i);
				if (idx != -1) {
					Vector2f diff = positions[i] - positions[idx];
					Vector2f n = normalize(diff);
					float vi = length(velocities[i]);
					velocities[i] = n * vi;
					vi = length(velocities[idx]);
					velocities[idx] = -1.0f * n * vi;
					positions[i] += velocities[i] * dt;
					positions[idx] += velocities[idx] * dt;
				}
			}
		}

		void separate(DataContainer& container,int posIndex,int velocityIndex,float radius,float dt) {
			Vector2f* position = container.getChannel<Vector2f>(posIndex);
			Vector2f* velocity = container.getChannel<Vector2f>(velocityIndex);
			for (int i = 0; i < container.size(); ++i) {
				int idx = findNearest(*position, radius, container,posIndex, i);
				if (idx != -1) {
					ID id = container.getID(idx);
					Vector2f p = container.get<Vector2f>(id,posIndex);
					Vector2f diff = *position - p;
					Vector2f n = normalize(diff);
					float vi = length(*velocity);
					*velocity = n * vi;
					vi = length(container.get<Vector2f>(id,velocityIndex));
					container.set<Vector2f>(id,velocityIndex,-1.0f * n * vi);
					*position += *velocity * dt;
					p += container.get<Vector2f>(id,velocityIndex) * dt;
					container.set<Vector2f>(id,posIndex,p);
				}
				++position;
				++velocity;
			}
		}

		void separateByRelaxation(DataContainer& container,int posIndex, float radius, float relaxation) {
			Vector2f* position = container.getChannel<Vector2f>(posIndex);
			for (int i = 0; i < container.size(); ++i) {
				int idx = findNearest(*position, radius, container,posIndex, i);
				if (idx != -1) {
					ID id = container.getID(idx);
					Vector2f p = container.get<Vector2f>(id,posIndex);
					Vector2f diff = *position - p;
					Vector2f n = normalize(diff);
					n *= 0.5f;
					*position += n;
					p -= n;
					container.set<Vector2f>(id,posIndex,p);
				}
				++position;
			}
		}

		// -------------------------------------------------------
		// find nearest enemy to current enemy
		// -------------------------------------------------------
		int findNearest(const Vector2f& pos, float radius, const Vector2f* positions,int num,int self) {
			int ret = -1;
			float min = 2000.0f * 2000.0f;
			for (int i = 0; i < num; ++i) {
				if (i != self) {
					Vector2f diff = pos - positions[i];
					float distSq = sqr_length(diff);
					if (distSq < (radius + radius)*(radius + radius)) {
						if (distSq < min) {
							ret = i;
							min = distSq;
						}
					}
				}
			}
			return ret;
		}
		
		int findNearest(const Vector2f& pos, float radius, const DataContainer& container,int posIndex,int self) {
			int ret = -1;
			float min = 2000.0f * 2000.0f;
			const Vector2f* positions = container.getChannel<Vector2f>(posIndex);
			for (int i = 0; i < container.size(); ++i) {
				if (i != self) {
					Vector2f diff = pos - *positions;
					float distSq = sqr_length(diff);
					if (distSq < (radius + radius)*(radius + radius)) {
						if (distSq < min) {
							ret = i;
							min = distSq;
						}
					}
				}
				++positions;
			}
			return ret;
		}
	}

}