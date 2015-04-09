#pragma once
#include <Vector.h>
#include <math.h>

namespace ds {

	namespace vector {

		void srt(const Vector2f& v, const Vector2f& u, const Vector2f& scale, float rotation, Vector2f* ret);

		Vector2f srt(const Vector2f& v,const Vector2f& u,float scaleX,float scaleY,float rotation);

		Vector3f srt(const Vector3f& v,const Vector3f& u,float scaleX,float scaleY,float rotation);

		void addRadial(Vector2f& v,float radius,float angle);

		Vector2f calculateRadial(const Vector2f& v,float radius,float angle);

		void rotate(Vector2f& v,float angle);

		float getAngle(const Vector2f& v1,const Vector2f& v2);

		float calculateRotation(const Vector2f& v);

		Vector2f getRadialVelocity(float angle,float velocity);

		void clamp(Vector2f& v, const Vector2f& min, const Vector2f& max);

	}
}
