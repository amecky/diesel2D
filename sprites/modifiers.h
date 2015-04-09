#pragma once
#include <Vector.h>
#include "..\utils\Color.h"
#include "..\lib\DataContainer.h"

namespace ds {

	namespace modifiers {

		void bounce(Vector2f* positions, Vector2f* velocities, int num, float left, float right, float bottom, float top);

		void tick(float* timers, int num, float dt);

		void dampVelocities(Vector2f* velocities, int num , float damping, float min);

		void move(Vector2f* positions, Vector2f* velocities, int num, float dt);

		void move(DataContainer& container,int posIdx,int velIdx,float dt);

		void rotate(float* angles, int num, float dt);

		float rotateTo(const Vector2f& position,const Vector2f& target);

		void lerpScale(Vector2f* scales, float* timers, int num, const Vector2f& min, const Vector2f& max, float ttl);

		void lerpColors(Color* colors, float* timers, int num, const Color& min, const Color& max, float ttl);

		void flashColors(Color* colors, float* timers, int num, const Color& startColor, const Color& endColor,float amplitude);

		void moveTowards(Vector2f* positions, int num, const Vector2f& target, float velocity, float dt);

		void moveTowards(Vector2f* positions,float* angles, int num, const Vector2f& target, float velocity, float dt);

		void follow(Vector2f* positions, float* angles, int num, const Vector2f& target, float velocity, float dt);

		void followRelative(Vector2f* positions, float* angles, int num, const Vector2f& target, float minDistance,float percentage, float dt);

		void handleKeyboard(Vector2f* pos,float* angle,float speed,float dt);

		void separate(Vector2f* positions, int num, float radius, float relaxation);

		void separate(Vector2f* positions, Vector2f* velocities,int num, float radius,float dt);

		int findNearest(const Vector2f& pos, float radius, const Vector2f* positions, int num, int self);

		void tick(DataContainer& container,int timerIndex, float dt);

		void rotate(DataContainer& container, int anlgeIndex, float dt);

		void separate(DataContainer& container, float radius, float dt);

		void separateByRelaxation(DataContainer& container,int posIndex, float radius, float relaxation);

		int findNearest(const Vector2f& pos, float radius, const ds::DataContainer& enemies,int posIndex, int self);

		void bounce(DataContainer& container,int posIdx,int velIdx,int radiusIdx,float left, float right, float bottom, float top);

	}

}