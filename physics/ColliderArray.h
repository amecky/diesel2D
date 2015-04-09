#pragma once
#include <Vector.h>
#include "..\renderer\render_types.h"
#include "..\lib\DataContainer.h"
#include "..\sprites\Sprite.h"

namespace ds {

	typedef unsigned int CID;

	struct Collision {

		Vector2f firstPos;
		Vector2f secondPos;
		SID firstSID;
		SID secondSID;
		CID firstID;
		CID secondID;
		int firstType;
		int secondType;

		const bool containsType(int type) const {
			if ( firstType == type ) {
				return true;
			}
			if ( secondType == type ) {
				return true;
			}
			return false;
		}
	};

	struct ColliderArrayIndex {
		CID id;
		unsigned short index;
		unsigned short next;
	};

	struct ColliderArray {

		int num;
		int total;
		ColliderArrayIndex* indices;
		CID* ids;
		SID* sids;
		Vector2f* positions;
		Vector2f* extents;
		int* types;
		char* buffer;

		unsigned short free_enqueue;
		unsigned short free_dequeue;

		ColliderArray() : num(0) , total(0) , buffer(0) {								
		}

		CID create(SID sid,const Vector2f& pos,const Vector2f& extent,int type) {
			ColliderArrayIndex &in = indices[free_dequeue];
			free_dequeue = in.next;
			in.index = num++;
			ids[in.index] = in.id;
			sids[in.index] = sid;
			positions[in.index] = pos;
			extents[in.index] = extent;			
			types[in.index] = type;			
			return in.id;
		}

		void moveTo(CID cid,const Vector2f& p) {
			ColliderArrayIndex &in = indices[cid];
			assert(in.index != USHRT_MAX);
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
			extents[in.index] = extents[next.index];			
			types[in.index] = types[next.index];			
			--num;
			indices[currentID & INDEX_MASK].index = in.index;
			in.index = USHRT_MAX;
			indices[free_enqueue].next = id & INDEX_MASK;
			free_enqueue = id & INDEX_MASK;
		}
	};

	namespace physics {

		void clear(ColliderArray& array);

		void debug(ColliderArray& array);

		void debug(const Collision& c);

		bool isInsideBox(const Vector2f& ray,const Vector2f& p,const Vector2f& dim);

		bool testBoxIntersection(const Vector2f& p1,const Vector2f& e1,const Vector2f& p2,const Vector2f& e2);
		
	}

}
