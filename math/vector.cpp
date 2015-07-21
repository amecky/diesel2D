#include "vector.h"
#include <math.h>
#include "GameMath.h"
#include <Vector.h>
#include "..\utils\Log.h"

namespace ds {

namespace vector {

	void rotate(Vector2f& v,float angle) {
		float xt = (v.x * cosf(angle)) + (v.y * sinf(angle));
		float yt = (v.y * cosf(angle)) - (v.x * sinf(angle));
		v.x = xt;
		v.y = yt;
	}

	//! Scale and rotate counter clock wise and translate
	/*
		\param v the center
		\param u the position
		\param scaleX the x scale
		\param scaleY the y scale
		\param rotation the rotation in radians
		\return a new Vector2f which is scaled and rotated and translated
	*/
	Vector2f srt(const Vector2f& v,const Vector2f& u,float scaleX,float scaleY,float rotation) {
		float sx = u.x * scaleX;
		float sy = u.y * scaleY;

		// rotation clock wise
		//float xt = cosf(rotation) * sx + sinf(rotation) * sy;
		//float yt = -sinf(rotation) * sx + cosf(rotation) * sy;

		// rotation counter clock wise
		//float xt = cosf(rotation) * sx - sinf(rotation) * sy;
		//float yt = sinf(rotation) * sx + cosf(rotation) * sy;

		float xt = math::fastCos(rotation) * sx - math::fastSin(rotation) * sy;
		float yt = math::fastSin(rotation) * sx + math::fastCos(rotation) * sy;

		xt += v.x;
		yt += v.y;

		return Vector2f(xt,yt);
	}

	void srt(const Vector2f& v, const Vector2f& u, const Vector2f& scale, float rotation,Vector2f* ret) {
		float sx = u.x * scale.x;
		float sy = u.y * scale.y;

		// rotation clock wise
		//float xt = cosf(rotation) * sx + sinf(rotation) * sy;
		//float yt = -sinf(rotation) * sx + cosf(rotation) * sy;

		// rotation counter clock wise
		//float xt = cosf(rotation) * sx - sinf(rotation) * sy;
		//float yt = sinf(rotation) * sx + cosf(rotation) * sy;

		float xt = math::fastCos(rotation) * sx - math::fastSin(rotation) * sy;
		float yt = math::fastSin(rotation) * sx + math::fastCos(rotation) * sy;

		xt += v.x;
		yt += v.y;

		ret->x = xt;
		ret->y = yt;
		//return Vector2f(xt, yt);
	}

	Vector3f srt(const Vector3f& v,const Vector3f& u,float scaleX,float scaleY,float rotation) {
		float sx = u.x * scaleX;
		float sy = u.y * scaleY;

		// rotation clock wise
		//float xt = cosf(rotation) * sx + sinf(rotation) * sy;
		//float yt = -sinf(rotation) * sx + cosf(rotation) * sy;

		// rotation counter clock wise
		//float xt = cosf(rotation) * sx - sinf(rotation) * sy;
		//float yt = sinf(rotation) * sx + cosf(rotation) * sy;

		float xt = math::fastCos(rotation) * sx - math::fastSin(rotation) * sy;
		float yt = math::fastSin(rotation) * sx + math::fastCos(rotation) * sy;

		xt += v.x;
		yt += v.y;

		return Vector3f(xt,yt,v.z);
	}

	
	void addRadial(Vector2f& v,float radius,float angle) {
		v.x += radius * math::fastCos(angle);
		v.y += radius * math::fastSin(angle);
	}

	// -PI < beta < PI   0 < phi < 2PI
	void addRadial(Vector3f& v, float radius, float beta, float phi) {
		v.x += radius * cos(phi) * sin(beta);
		v.y += radius * sin(phi) * sin(beta);
		v.z += radius * cos(beta);
	}

	Vector2f calculateRadial(const Vector2f& v,float radius,float angle) {
		Vector2f ret;
		//float ra = math::reflect(angle);
		float ra = angle;
		ret.x = v.x + radius * math::fastCos(ra);
		ret.y = v.y + radius * math::fastSin(ra);
		return ret;
	}

	float calculateRotation(const Vector2f& v) {
		Vector2f vn = normalize(v);
		if (vn != V2_RIGHT) {
			float dt = math::clamp(dot(vn, V2_RIGHT),-1.0f,1.0f);
			float tmp = acos(dt);
			float cross = -1.0f * vn.y;
			if (cross > 0.0f) {
				tmp = 2.0f * PI - tmp;
			}
			return tmp;
		}
		else {
			return 0.0f;
		}
	}

	float getAngle(const Vector2f& v1,const Vector2f& v2)  {
		Vector2f diff = v2 - v1;
		return calculateRotation(diff);
		/*
		Vector2f vn1 = normalize(v1);
		Vector2f vn2 = normalize(v2);
		if ( vn1 != vn2 ) {			
			float dt = dot(vn1,vn2);		
			if ( dt < -1.0f ) {
				dt = -1.0f;
			}
			if ( dt > 1.0f ) {
				dt = 1.0f;
			}
			float tmp = acos(dt);
			float cross = (vn1.x * vn2.y) - (vn2.x * vn1.y);
			if ( cross < 0.0f ) {
				tmp = 2.0f * PI - tmp;
			}
			return tmp;		
		}
		else {
			return 0.0f;
		}
		*/
	}

	Vector2f getRadialVelocity(float angle,float velocity) {
		float vx = math::fastCos(angle) * velocity;
		float vy = math::fastSin(angle) * velocity;
		return Vector2f(vx,vy);
	}

	void clamp(Vector2f& v, const Vector2f& min, const Vector2f& max) {
		if (v.x > max.x) {
			v.x = max.x;
		}
		if (v.x < min.x) {
			v.x = min.x;
		}
		if (v.y > max.y) {
			v.y = max.y;
		}
		if (v.y < min.y) {
			v.y = min.y;
		}
	}
	
}

}