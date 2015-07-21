#include "ColliderArray.h"
#include "..\utils\Log.h"

namespace ds {

	namespace physics {
		/*
		void clear(ColliderArray& array) {
			if ( array.buffer != 0 ) {
				for ( unsigned short i = 0; i < array.total; ++i ) {
					array.indices[i].id = i;
					array.indices[i].next = i + 1;
				}
				array.num = 0;
				array.free_dequeue = 0;
				array.free_enqueue = array.total - 1;
			}
		}
		*/
		void debug(ColliderArray<Vector2f>& array) {
			for ( int i = 0; i < array.num; ++i ) {
				LOG << i << " : id: " << array.ids[i] << " pos: " << DBG_V2(array.positions[i]);
			}
		}

		void debug(ColliderArray<Vector3f>& array) {
			for (int i = 0; i < array.num; ++i) {
				LOG << i << " : id: " << array.ids[i] << " pos: " << DBG_V3(array.positions[i]);
			}
		}

		void debug(const Collision& c) {
			LOG << "first  - id: " << c.firstColliderID << " sid: " << c.firstSID << " type: " << c.firstType << " pos: " << DBG_V2(c.firstPos);
			LOG << "second - id: " << c.secondColliderID << " sid: " << c.secondSID << " type: " << c.secondType << " pos: " << DBG_V2(c.secondPos);
		}

		bool isInsideBox(const Vector2f& ray,const Vector2f& p,const Vector2f& dim) {
			float top = p.y - dim.y;
			float left = p.x - dim.x;
			float right = p.x + dim.x;
			float bottom = p.y + dim.y;
			if( right < ray.x || left > ray.x ) return false;
			if( bottom < ray.y || top > ray.y ) return false;
			return true;
		}

		bool testBoxIntersection(const Vector2f& p1,const Vector2f& e1,const Vector2f& p2,const Vector2f& e2) {
			float top = p1.y - e1.y * 0.5f;
			float left = p1.x - e1.x * 0.5f;
			float right = p1.x + e1.x * 0.5f;
			float bottom = p1.y + e1.y * 0.5f;

			float otherTop = p2.y - e2.y * 0.5f;
			float otherLeft = p2.x - e2.x * 0.5f;
			float otherRight = p2.x + e2.x * 0.5f;
			float otherBottom = p2.y + e2.y * 0.5f;
		
			if( right < otherLeft || left > otherRight ) return false;
			if( bottom < otherTop || top > otherBottom ) return false;
			return true;
		}

		bool testBoxIntersection(const Vector3f& p1, const Vector3f& e1, const Vector3f& p2, const Vector3f& e2) {
			float top = p1.y - e1.y * 0.5f;
			float left = p1.x - e1.x * 0.5f;
			float right = p1.x + e1.x * 0.5f;
			float bottom = p1.y + e1.y * 0.5f;

			float otherTop = p2.y - e2.y * 0.5f;
			float otherLeft = p2.x - e2.x * 0.5f;
			float otherRight = p2.x + e2.x * 0.5f;
			float otherBottom = p2.y + e2.y * 0.5f;

			if (right < otherLeft || left > otherRight) return false;
			if (bottom < otherTop || top > otherBottom) return false;
			return true;
		}

		// http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=3
		bool sweepTest(const AABBox& b1, const Vector2f& prev1, const AABBox& b2, const Vector2f& prev2, float* u0, float *u1) {
			const AABBox A(prev1, b1.extent);//previous state of AABB A
			const AABBox B(prev2, b2.extent);//previous state of AABB B
			const Vector3f va = b1.position - prev1;//displacement of A
			const Vector3f vb = b2.position - prev2;//displacement of B
			//the problem is solved in A's frame of reference
			Vector3f v = vb - va;
			//relative velocity (in normalized time)
			Vector3f u_0(0, 0, 0);
			//first times of overlap along each axis
			Vector3f u_1(1, 1, 1);
			//last times of overlap along each axis

			//check if they were overlapping 
			// on the previous frame
			if (A.overlaps(B)) {
				*u0 = *u1 = 0.0f;
				return true;
			}

			//find the possible first and last times
			//of overlap along each axis
			for (long i = 0; i < 3; ++i)	{
				if (A.max_value(i)<B.min_value(i) && v[i]<0) {
					u_0[i] = (A.max_value(i) - B.min_value(i)) / v[i];
				}
				else if (B.max_value(i)<A.min_value(i) && v[i]>0) {
					u_0[i] = (A.min_value(i) - B.max_value(i)) / v[i];
				}
				if (B.max_value(i)>A.min_value(i) && v[i]<0) {
					u_1[i] = (A.min_value(i) - B.max_value(i)) / v[i];
				}
				else if (A.max_value(i)>B.min_value(i) && v[i]>0) {
					u_1[i] = (A.max_value(i) - B.min_value(i)) / v[i];
				}
			}
			//possible first time of overlap
			*u0 = max(u_0.x, max(u_0.y, u_0.z));
			//possible last time of overlap
			*u1 = min(u_1.x, min(u_1.y, u_1.z));
			//they could have only collided if
			//the first time of overlap occurred
			//before the last time of overlap
			return *u0 <= *u1;
		}

		bool testAxisStatic(Vector2f axis, float minA, float maxA, float minB, float maxB, Vector2f& mtvAxis, float& mtvDistance) {
			// [Separating Axis Theorem]
			// • Two convex shapes only overlap if they overlap on all axes of separation
			// • In order to create accurate responses we need to find the collision vector (Minimum Translation Vector)   
			// • Find if the two boxes intersect along a single axis 
			// • Compute the intersection interval for that axis
			// • Keep the smallest intersection/penetration value
			float axisLengthSquared = dot(axis, axis);
			// If the axis is degenerate then ignore
			if (axisLengthSquared < 1.0e-8f) {
				return true;
			}
			// Calculate the two possible overlap ranges
			// Either we overlap on the left or the right sides
			float d0 = (maxB - minA);   // 'Left' side
			float d1 = (maxA - minB);   // 'Right' side
			// Intervals do not overlap, so no intersection
			if (d0 <= 0.0f || d1 <= 0.0f) {
				return false;
			}
			// Find out if we overlap on the 'right' or 'left' of the object.
			float overlap = (d0 < d1) ? d0 : -d1;
			// The mtd vector for that axis
			Vector2f sep = axis * (overlap / axisLengthSquared);
			// The mtd vector length squared
			float sepLengthSquared = dot(sep, sep);
			// If that vector is smaller than our computed Minimum Translation Distance use that vector as our current MTV distance
			if (sepLengthSquared < mtvDistance) {
				mtvDistance = sepLengthSquared;
				mtvAxis = sep;
			}
			return true;
		}

		// http://gamedev.stackexchange.com/questions/32807/collision-resolve-sphere-aabb-and-aabb-aabb
		bool testStaticAABBAABB(AABBox a, AABBox b, Vector2f* normal,float* penetration) {

			// [Minimum Translation Vector]
			float mtvDistance = 20000.0f;             // Set current minimum distance (max float value so next value is always less)
			Vector2f mtvAxis;// = new Vector3();                // Axis along which to travel with the minimum distance

			// [Axes of potential separation]
			// • Each shape must be projected on these axes to test for intersection:
			//          
			// (1, 0, 0)                    A0 (= B0) [X Axis]
			// (0, 1, 0)                    A1 (= B1) [Y Axis]
			// (0, 0, 1)                    A1 (= B2) [Z Axis]

			// [X Axis]
			if (!testAxisStatic(Vector2f(1, 0), a.min.x, a.max.x, b.min.x, b.max.x, mtvAxis, mtvDistance)) {
				return false;
			}

			// [Y Axis]
			if (!testAxisStatic(Vector2f(0, 1), a.min.y, a.max.y, b.min.y, b.max.y, mtvAxis, mtvDistance)) {
				return false;
			}

			// [Z Axis]
			//if (!testAxisStatic(Vector3f(0, 0, 1), a.min.z, a.max.z, b.min.z, b.max.z, mtvAxis, mtvDistance))
			//{
				//return false;
			//}

			//contact->intersecting = true;

			// Calculate Minimum Translation Vector (MTV) [normal * penetration]
			*normal = normalize(mtvAxis);

			// Multiply the penetration depth by itself plus a small increment
			// When the penetration is resolved using the MTV, it will no longer intersect
			*penetration = sqrt(mtvDistance) * 1.001f;

			return true;
		}

		bool solveQuadraticFormula(float a,float b,float c,float* r1,float* r2)	{
			float q = b * b - 4 * a * c;
			if (q >= 0)	{
				float sq = sqrt(q);
				float d = 1 / (2 * a);
				*r1 = (-b + sq) * d;
				*r2 = (-b - sq) * d;
				return true;//real roots
			}
			else {
				return false;//complex roots
			}
		}

		// https://musingsofninjarat.wordpress.com/tag/continuous-collision-detection/
		bool testCircleSweepIntersection(float ra,const Vector2f& A0,const Vector2f& A1,float rb,const Vector2f& B0,const Vector2f&	B1,float* u0,float* u1)	{
			const Vector2f va = A1 - A0;
			if (sqr_length(va) == 0.0f) {
				return testCircleIntersection(A1, ra, B1, rb);
			}
			//vector from A0 to A1
			const Vector2f vb = B1 - B0;
			if (sqr_length(vb) == 0.0f) {
				return testCircleIntersection(A1, ra, B1, rb);
			}
			//vector from B0 to B1
			const Vector2f AB = B0 - A0;
			//vector from A0 to B0
			const Vector2f vab = vb - va;
			//relative velocity (in normalized time)
			float rab = ra + rb;
			float a = dot(vab,vab);
			//u*u coefficient
			float b = 2.0f * dot(vab,AB);
			//u coefficient
			float c = dot(AB,AB) - rab*rab;
			//constant term
			//check if they're currently overlapping
			if (dot(AB,AB) <= rab*rab) {
				*u0 = 0;
				*u1 = 0;
				return true;
			}
			//check if they hit each other 
			// during the frame
			if (solveQuadraticFormula(a, b, c, u0, u1)) {
				// 0 <= u0 <= 1 0 <= u1 <= 1 -> otherwise they will not collide within this frame
				if (*u0 >= 00.f && *u0 <= 1.0f && *u1 >= 0.0f && *u1 <= 1.0f) {
					if (*u0 > *u1) {
						float tmp = *u0;
						*u0 = *u1;
						*u1 = tmp;
					}
					return true;
				}
			}
			return false;
		}



		bool testCircleIntersection(const Vector2f& p1, float r1, const Vector2f& p2, float r2) {
			/*
			collisionPointX = ((p1.x * r2) + (p2.x * r1)) / (r1 + r2);
			collisionPointY = ((p1.y * r2) + (p2.y * r1)) / (r1 + r2);
			*/
			Vector2f d = p2 - p1;
			if (sqr_length(d) < r1 * r2) {
				return true;
			}
			return false;
		}

	}
}