#pragma once
#include <Vector.h>
#include <math.h>

namespace ds {

	namespace vector {

		Vector2f srt(const Vector2f& v,const Vector2f& u,float scaleX,float scaleY,float rotation);

		void addRadial(Vector2f& v,float radius,float angle);

		Vector2f calculateRadial(const Vector2f& v,float radius,float angle);

		void rotate(Vector2f& v,float angle);

		float getAngle(const Vector2f& v1,const Vector2f& v2);

		Vector2f getRadialVelocity(float angle,float velocity);

	}
}
