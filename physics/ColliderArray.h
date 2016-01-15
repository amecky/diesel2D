#pragma once
#include <Vector.h>
#include "..\renderer\render_types.h"
#include "..\sprites\Sprite.h"

namespace ds {

	typedef unsigned int CID;

	template<class T>
	struct CollisionTemplate {

		T firstPos;
		T secondPos;
		SID firstSID;
		SID secondSID;
		CID firstColliderID;
		CID secondColliderID;
		int firstType;
		int secondType;
		T norm;
		float distance;

		const bool containsType(int type) const {
			if ( firstType == type ) {
				return true;
			}
			if ( secondType == type ) {
				return true;
			}
			return false;
		}

		SID getSIDByType(int type) const {
			if (firstType == type) {
				return firstSID;
			}
			return secondSID;
		}
	};

	typedef CollisionTemplate<Vector2f> Collision;
	typedef CollisionTemplate<Vector3f> Collision3D;

	struct ColliderArrayIndex {
		CID id;
		unsigned short index;
		unsigned short next;
	};

	enum ColliderShape {

		CS_BOX,
		CS_CIRCLE
	};

	template<class T>
	struct ColliderArray {

		int num;
		int total;
		ColliderArrayIndex* indices;
		CID* ids;
		SID* sids;
		T* positions;
		T* previous;
		T* extents;
		int* types;
		int* layers;
		ColliderShape* shapeTypes;
		char* buffer;

		unsigned short free_enqueue;
		unsigned short free_dequeue;

		ColliderArray() : num(0) , total(0) , buffer(0) {								
		}
		
		CID create(SID sid,const T& pos,const T& extent,int type,int layer,ColliderShape shape = CS_CIRCLE) {
			ColliderArrayIndex &in = indices[free_dequeue];
			free_dequeue = in.next;
			in.index = num++;
			ids[in.index] = in.id;
			sids[in.index] = sid;
			positions[in.index] = pos;
			previous[in.index] = pos;
			extents[in.index] = extent;			
			types[in.index] = type;			
			layers[in.index] = layer;
			shapeTypes[in.index] = shape;
			return in.id;
		}

		void moveTo(CID cid,const T& p) {
			ColliderArrayIndex &in = indices[cid];
			assert(in.index != USHRT_MAX);
			previous[in.index] = positions[in.index];
			positions[in.index] = p;
		}

		void remove(CID id) {
			ColliderArrayIndex &in = indices[id & INDEX_MASK];
			assert(in.index != USHRT_MAX);
			ID currentID = ids[num-1];
			ColliderArrayIndex& next = indices[currentID & INDEX_MASK];
			ids[in.index] = ids[next.index];
			sids[in.index] = sids[next.index];
			positions[in.index] = positions[next.index];
			previous[in.index] = previous[next.index];
			extents[in.index] = extents[next.index];			
			types[in.index] = types[next.index];			
			layers[in.index] = layers[next.index];
			shapeTypes[in.index] = shapeTypes[next.index];
			--num;
			indices[currentID & INDEX_MASK].index = in.index;
			in.index = USHRT_MAX;
			indices[free_enqueue].next = id & INDEX_MASK;
			free_enqueue = id & INDEX_MASK;
		}

		void clear() {
			if (buffer != 0) {
				for (unsigned short i = 0; i < total; ++i) {
					indices[i].id = i;
					indices[i].next = i + 1;
				}
				num = 0;
				free_dequeue = 0;
				free_enqueue = total - 1;
			}
		}
		
	};

	namespace physics {

		void debug(ColliderArray<Vector2f>& array);

		void debug(ColliderArray<Vector3f>& array);

		void debug(const Collision& c);

		bool isInsideBox(const Vector2f& ray,const Vector2f& p,const Vector2f& dim);

		bool testCircleSweepIntersection(float ra, const Vector2f& A0, const Vector2f& A1, float rb, const Vector2f& B0, const Vector2f&	B1, float* u0, float* u1);

		bool testCircleIntersection(const Vector2f& p1, float r1, const Vector2f& p2, float r2);

		bool testBoxIntersection(const Vector2f& p1,const Vector2f& e1,const Vector2f& p2,const Vector2f& e2);

		bool testBoxIntersection(const Vector3f& p1, const Vector3f& e1, const Vector3f& p2, const Vector3f& e2);

		bool sweepTest(const AABBox& b1, const Vector3f& prev1, const AABBox& b2, const Vector3f& prev2, float* u0, float *u1);

		bool testStaticAABBAABB(AABBox a, AABBox b, Vector3f* normal, float* penetration);
		
	}

}
