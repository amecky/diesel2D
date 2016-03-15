#pragma once
#include "..\sprites\SpriteArray.h"
#include "ColliderArray.h"
#include <map>
#include "..\sprites\SpriteBatch.h"
#include "..\math\Bitset.h"
#include "..\lib\collection_types.h"

namespace ds {

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

	typedef Array<IgnoredCollision> IgnoredCollisions;

	struct PotentialCollider {
		CID first;
		CID second;
		int firstIndex;
		int secondIndex;
	};

	class PotentialColliders {

	public:
		PotentialColliders() {}
		~PotentialColliders() {}
		void reset();
		void add(int firstIndex,CID first, int firstType,int secondIndex, CID second, int secondType);
		int size() const;
		void ignore(int firstType, int secondType);
		const PotentialCollider& get(int index) const;
	private:
		bool contains(CID first, CID second) const;
		bool shouldBeIgnored(int firstType, int secondType);
		//Array<PotentialCollider> _colliders;
		PotentialCollider _colliders[4096];
		int num;
		IgnoredCollisions _ignored;
	};

	class PhysicalWorld {

	public:
		PhysicalWorld();
		~PhysicalWorld();
		void ignore(int firstType, int secondType);
		void ignoreLayer(int layer);
		void setDataPtr(SpriteArray* sprites);
		void attachCollider(SID sid, const Vector2f& extent, int type,int layer);
		void attachCollider(SID sid, int type,int layer);
		void attachBoxCollider(SID sid, int type, int layer);
		bool hasCollisions() {
			return _collisions.size() > 0;
		}
		int getNumCollisions() {
			return _collisions.size();
		}
		const Collision& getCollision(int idx) const {
			return _collisions[idx];
		}
		void tick(float dt);
		void debug();
		void debug(SID sid);
		void save(const ReportWriter& writer);
		void remove(SID id);
		void drawColliders(const Texture& texture);
	private:
		bool intersects(int firstIndex, int secondIndex);
		//bool shouldBeIgnored(int firstType, int secondType);
		void allocateCollider(int size);
		void checkCollisions();
		void checkCollisions(int currentIndex, const Vector2f& pos, const Vector2f& extent);
		bool containsCollision(CID firstID, CID secondID);
		SpriteArray* m_Sprites;
		ColliderArray<Vector2f> m_ColliderData;
		Array<Collision> _collisions;
		IgnoredCollisions m_Ignored;
		Bits _ignoredLayers;
		PotentialColliders _potentialColliders;
	};

}

