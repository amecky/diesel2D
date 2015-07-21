#pragma once
#include "..\sprites\SpriteArray.h"
#include "ColliderArray.h"
#include <map>
#include "..\sprites\SpriteBatch.h"

namespace ds {

	class PhysicalWorld {

	struct IgnoredCollision {

		int firstType;
		int secondType;

		bool matches(int first, int second) {
			if (first == firstType && second == secondType) {
				return true;
			}
			if (second == firstType && first == secondType) {
				return true;
			}
			return false;
		}
	};

	typedef std::map<SID, CID> ColliderMap;
	typedef std::vector<IgnoredCollision> IgnoredCollisions;
	
	public:
		PhysicalWorld();
		~PhysicalWorld();
		void ignore(int firstType, int secondType);
		void setDataPtr(SpriteArray* sprites);
		void attachCollider(SID sid, const Vector2f& extent, int type);
		void attachCollider(SID sid, int type);
		bool hasCollisions() {
			return m_NumCollisions > 0;
		}
		int getNumCollisions() {
			return m_NumCollisions;
		}
		const Collision& getCollision(int idx) const {
			return m_Collisions[idx];
		}
		void tick(float dt);
		void debug();
		void remove(SID id);
		void drawColliders(const Texture& texture);
	private:
		bool intersects(int firstIndex, int secondIndex);
		bool shouldBeIgnored(int firstType, int secondType);
		void allocateCollider(int size);
		void checkCollisions();
		void checkCollisions(int currentIndex, const Vector2f& pos, const Vector2f& extent);
		bool containsCollision(CID firstID, CID secondID);
		SpriteArray* m_Sprites;
		ColliderMap m_ColliderMap;
		ColliderArray<Vector2f> m_ColliderData;
		Collision m_Collisions[256];
		IgnoredCollisions m_Ignored;
		int m_NumCollisions;
	};

}

