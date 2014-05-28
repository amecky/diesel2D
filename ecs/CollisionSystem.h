#pragma once
#include <vector>
#include "Actor.h"
#include "..\lib\DataArray.h"

namespace ds {

// -------------------------------------------------------
// Collision
// -------------------------------------------------------
struct ActorCollision {
	ID id;
	Vector2f firstPosition;
	Vector2f secondPosition;
	ID firstId;
	ID secondId;
	uint32 firstType;
	uint32 secondType;
};


class CollisionCallback {

public:
	virtual void onCollision(const ActorCollision& collision) = 0;
};


// -------------------------------------------------------
// CollisionObject
// -------------------------------------------------------
struct ActorCollider {
	Vector2f prevPosition;
	Vector2f extent;
	ID id;
	uint32 type;
	ID actorID;
};

// -------------------------------------------------------
// CollisionManager
// -------------------------------------------------------
class CollisionSystem {

typedef DataArray<ActorCollider> ActorCollisionObjects;
typedef DataArray<ActorCollision> ActorCollisions;

struct IgnoreCollision {

	uint32 firstType;
	uint32 secondType;

	IgnoreCollision() : firstType(-1) , secondType(-1) {}

	bool isUsed() {
		return firstType != -1 || secondType != -1;
	}

	bool matches(int ft,int st) {
		if ( firstType == ft && secondType == st ) {
			return true;
		}
		if ( firstType == st && secondType == ft ) {
			return true;
		}
		return false;
	}
};

typedef std::vector<IgnoreCollision> Ignores;


public:
	CollisionSystem();	
	virtual ~CollisionSystem();
	ID add(ID actorID,float radius,uint32 type);
	void remove(ID id);
	int checkIntersections();
	void reset();
	const ActorCollision& getCollision(uint32 index) const;
	const uint32 getCollisionCounter() const {
		return m_Collisions.numObjects;
	}
	bool bufferContains(int firstID,int secondID);
	void addIgnore(int firstType,int secondType) {
		IgnoreCollision ic;
		ic.firstType = firstType;
		ic.secondType = secondType;
		m_Ignores.push_back(ic);
	}
	bool shouldIgnore(int firstType,int secondType) {
		for ( size_t i = 0; i < m_Ignores.size(); ++i ) {
			if ( m_Ignores[i].matches(firstType,secondType)) {
				return true;
			}
		}
		return false;
	}
	void debug();
	void setActorArray(DataArray<Actor,MAX_ITEMS>* actors) {
		m_Actors = actors;
	}
private:
	bool equals(const ActorCollider& first,const ActorCollider& second);
	CollisionSystem(const CollisionSystem& orig);
	bool intersectCircleCircle(const ActorCollider& first,const ActorCollider& second);
	bool addCollision(ActorCollider& first,ActorCollider& second);
	ActorCollisionObjects m_Objects;
	ActorCollisions m_Collisions;
	Ignores m_Ignores;
	int m_Counter;
	DataArray<Actor,MAX_ITEMS>* m_Actors;
};

namespace coll {

	bool containsType(const ActorCollision& collision,int type);

	bool matches(const ActorCollision& collision,int firstType,int secondType);

	int getIDByType(const ActorCollision& collision,int type);

	Vector2f getPositionByType(const ActorCollision& collision,int type);
}

}
