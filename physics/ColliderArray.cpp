#include "ColliderArray.h"
#include "..\utils\Log.h"

namespace ds {

	namespace physics {

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

		void debug(ColliderArray& array) {
			for ( int i = 0; i < array.num; ++i ) {
				LOG << i << " : id: " << array.ids[i] << " pos: " << DBG_V2(array.positions[i]);
			}
		}

		void debug(const Collision& c) {
			LOG << "first  - id: " << c.firstID << " sid: " << c.firstSID << " type: " << c.firstType << " pos: " << DBG_V2(c.firstPos);
			LOG << "second - id: " << c.secondID << " sid: " << c.secondSID << " type: " << c.secondType << " pos: " << DBG_V2(c.secondPos);
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

	}
}