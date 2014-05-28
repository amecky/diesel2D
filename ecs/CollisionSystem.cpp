#include "CollisionSystem.h"
#include "..\utils\Log.h"

namespace ds {

CollisionSystem::CollisionSystem() {}

CollisionSystem::CollisionSystem(const CollisionSystem& orig) {}

CollisionSystem::~CollisionSystem() {}

const ActorCollision& CollisionSystem::getCollision(uint32 index) const {
	return m_Collisions.objects[index];
}

bool CollisionSystem::bufferContains(int firstID, int secondID) {
	for ( uint32 i = 0; i < m_Collisions.numObjects; ++i ) {
		ActorCollision& col = m_Collisions.objects[i];
        if ( col.firstId == firstID && col.secondId == secondID ) {
            return true;
        }
        if ( col.firstId == secondID && col.secondId == firstID ) {
            return true;
        }
    }
    return false;
}

void CollisionSystem::reset() {
    m_Collisions.clear();
}

// -------------------------------------------------------
// Add circle
// -------------------------------------------------------
ID CollisionSystem::add(ID actorID,float radius,uint32 type) {
	ID id = m_Objects.add();
    ActorCollider& aco = m_Objects.get(id);
	aco.extent = Vector2f(radius,0.0f);
	aco.type = type;
	Actor& actor = m_Actors->get(actorID);
	aco.prevPosition = actor.position;
	aco.actorID = actorID;
	return id;
}

// -------------------------------------------------------
// Remove
// -------------------------------------------------------
void CollisionSystem::remove(ID id) {
	m_Objects.remove(id);
}

bool CollisionSystem::equals(const ActorCollider& first,const ActorCollider& second) {
	if ( first.id != second.id ) {
		return false;
	}
	if ( first.type != second.type ) {
		return false;
	}
	return true;
}
// -------------------------------------------------------
// Check intersections
// -------------------------------------------------------
int CollisionSystem::checkIntersections() {
	int ret = 0;
	for ( uint32 i = 0; i < m_Objects.numObjects; ++i ) {
		ActorCollider& aco = m_Objects.objects[i];
		for ( uint32 j = 0; j < m_Objects.numObjects; ++j ) {
			ActorCollider& other = m_Objects.objects[j];
			if ( !equals(aco,other) ) {
				//if ( !shouldIgnore(object->type,checkObject->type) && object->type != checkObject->type ) {
				if ( intersectCircleCircle(aco,other)) {
					if ( addCollision(aco,other) ) {
						++ret;
					}
				}
				Actor& otherActor = m_Actors->get(other.actorID);
				other.prevPosition = otherActor.position;
			}
			Actor& actor = m_Actors->get(aco.actorID);
			other.prevPosition = actor.position;
			aco.prevPosition = actor.position;
		}
    }
	return ret;
}

bool CollisionSystem::addCollision(ActorCollider& first, ActorCollider& second) {	
    if ( !bufferContains(first.id,second.id) ) {
		ID id = m_Collisions.add();
		ActorCollision& ac = m_Collisions.get(id);
		ac.firstId = first.actorID;
		ac.secondId = second.actorID;
		ac.firstPosition = m_Actors->get(first.actorID).position;
		ac.secondPosition = m_Actors->get(second.actorID).position;
		ac.firstType = first.type;
		ac.secondType = second.type;
		return true;
    }    
	return false;
}

// -------------------------------------------------------
// Intersect circle to circle
// -------------------------------------------------------
bool CollisionSystem::intersectCircleCircle(const ActorCollider& first, const ActorCollider& second) {
    Vector2f diff = m_Actors->get(first.actorID).position - m_Actors->get(second.actorID).position;
    float distSq = sqr_length(diff);
    float r1 = first.extent.x;
    float r2 = second.extent.x;
    if ( distSq == 0 && r1 == r2 ) {
        return true;
    }
    if ( distSq > ( r1 +r2)*(r1+r2)) {
        return false;
    }
    return true;
}

void CollisionSystem::debug() {
	for ( uint32 i = 0; i < m_Objects.numObjects; ++i ) {
		ActorCollider& aco = m_Objects.objects[i];
		Actor& actor = m_Actors->get(aco.actorID);
		LOG << i << " id: " << aco.id << " actorID: " << aco.actorID << " type: " << aco.type << " position: " << DBG_V2(actor.position) << " radius: " << aco.extent.x;
	}
}

namespace coll {

	bool containsType(const ActorCollision& collision,int type) {
		return collision.firstType == type || collision.secondType == type;
	}

	bool isBetween(const ActorCollision& collision,ID firstID,ID secondID) {
		if ( collision.firstId == firstID && collision.secondId == secondID) {
			return true;
		}
		if ( collision.firstId == secondID && collision.secondId == firstID ) {
			return true;
		}
		return false;
	}

	bool matches(const ActorCollision& collision,int firstType,int secondType) {
		if ( collision.firstType == firstType && collision.secondType == secondType ) {
			return true;
		}
		if ( collision.firstType == secondType && collision.secondType == firstType ) {
			return true;
		}
		return false;
	}

	int getIDByType(const ActorCollision& collision,int type) {
		if ( collision.firstType == type ) {
			return collision.firstId;
		}
		return collision.secondId;
	}

	Vector2f getPositionByType(const ActorCollision& collision,int type) {
		if ( collision.firstType == type ) {
			return collision.firstPosition;
		}
		return collision.secondPosition;
	}
}

}
