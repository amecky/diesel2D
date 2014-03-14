#include "SpriteCollisionManager.h"
#include "..\utils\Log.h"

namespace ds {

SpriteCollisionManager::SpriteCollisionManager() : m_Counter(0) {
    m_ColBuffer = new char[COLL_BUFFER_SIZE];
}

SpriteCollisionManager::SpriteCollisionManager(const SpriteCollisionManager& orig) {
}

SpriteCollisionManager::~SpriteCollisionManager() {
	SpriteCollisionObjects::iterator it = m_Objects.begin();
	while ( it != m_Objects.end()) {
		delete (*it);
		it = m_Objects.erase(it);
	}
    delete m_ColBuffer;
}

bool SpriteCollisionManager::getCollision(int index,SpriteCollision& collision) const {
    if ( index < m_BufferCounter ) {
        char* buffer = m_ColBuffer + index * COLL_SIZE;
        SpriteCollision col = *(SpriteCollision*)(buffer);
        collision.firstId = col.firstId;
        collision.secondId = col.secondId;
        collision.firstPosition = col.firstPosition;
		collision.secondPosition = col.secondPosition;
		collision.firstType = col.firstType;
		collision.secondType = col.secondType;
        return true;
    }
    return false;
}

bool SpriteCollisionManager::bufferContains(int firstID, int secondID) {
    char* buffer = m_ColBuffer;
    for ( int i = 0; i < m_BufferCounter;++i) {
        SpriteCollision col = *(SpriteCollision*)(buffer);
        if ( col.firstId == firstID && col.secondId == secondID ) {
            return true;
        }
        if ( col.firstId == secondID && col.secondId == firstID ) {
            return true;
        }
        buffer += COLL_SIZE;
    }
    return false;
}

void SpriteCollisionManager::reset() {
    m_Size = 0;
    m_BufferCounter = 0;
}

// -------------------------------------------------------
// Add circle
// -------------------------------------------------------
int SpriteCollisionManager::add(SpriteObject* sprite,int type) {
    int idx = m_Counter;
    SpriteCollisionObject* co = new SpriteCollisionObject;
    co->prevPosition = sprite->getPosition();
    co->extent.x = sprite->getRadius();
    co->extent.y = 0.0f;
	co->type = type;
    co->id = sprite->getID();
	co->sprite = sprite;
    m_Objects.push_back(co);
    ++m_Counter;
    return idx;
}

// -------------------------------------------------------
// Remove
// -------------------------------------------------------
void SpriteCollisionManager::remove(SpriteObject* entity,int type) {
	std::vector<SpriteCollisionObject*>::iterator it = m_Objects.begin();
	while ( it != m_Objects.end() ) {
		if ( (*it)->sprite->getID() == entity->getID() && (*it)->type == type ) {
			delete (*it);
			it = m_Objects.erase(it);
		}
		else {
			++it;
		}
	}
}

bool SpriteCollisionManager::equals(const SpriteCollisionObject* first,const SpriteCollisionObject* second) {
	if ( first->id != second->id ) {
		return false;
	}
	if ( first->type != second->type ) {
		return false;
	}
	return true;
}
// -------------------------------------------------------
// Check intersections
// -------------------------------------------------------
int SpriteCollisionManager::checkIntersections() {
	int ret = 0;
    for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
        SpriteCollisionObject* object = m_Objects[i];
		if ( object->sprite->isActive() ) {
			for ( std::size_t j = 0; j < m_Objects.size(); ++j ) {
				SpriteCollisionObject* checkObject = m_Objects[j];   
				if ( checkObject->sprite->isActive() && !equals(object,checkObject)) {
					if ( !shouldIgnore(object->type,checkObject->type)) {
						if ( intersectCircleCircle(object,checkObject)) {
							if ( addCollision(object,checkObject) ) {
								++ret;
							}
						}
					}				
					checkObject->prevPosition = object->sprite->getPosition();
				}
			}
			object->prevPosition = object->sprite->getPosition();
		}
    }
	return ret;
}

bool SpriteCollisionManager::addCollision(SpriteCollisionObject* first, SpriteCollisionObject* second) {	
    if ( !bufferContains(first->sprite->getID(),second->sprite->getID()) ) {
		if ( m_Size < COLL_BUFFER_SIZE ) {
			char* buffer = m_ColBuffer + m_Size;
			(*(SpriteCollision*)(buffer)).firstId = first->sprite->getID();
			(*(SpriteCollision*)(buffer)).secondId = second->sprite->getID();
			(*(SpriteCollision*)(buffer)).firstPosition = first->sprite->getPosition();
			(*(SpriteCollision*)(buffer)).secondPosition = second->sprite->getPosition();
			(*(SpriteCollision*)(buffer)).firstType = first->type;
			(*(SpriteCollision*)(buffer)).secondType = second->type;
			m_Size += COLL_SIZE;
			++m_BufferCounter;
			return true;
		}
    }    
	return false;
}

bool SpriteCollisionManager::intersectCircleCircle(const SpriteCollisionObject* first, const SpriteCollisionObject* second) {
    Vector2f diff = first->sprite->getPosition() - second->sprite->getPosition();
    float distSq = sqr_length(diff);
    float r1 = first->extent.x;
    float r2 = second->extent.x;
    if ( distSq == 0 && r1 == r2 ) {
        return true;
    }
    if ( distSq > ( r1 +r2)*(r1+r2)) {
        return false;
    }
    return true;
}

SpriteCollisionObject* SpriteCollisionManager::findObject(int id) {
    for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
        SpriteCollisionObject* object = m_Objects[i];
        if ( object->id == id ) {
            return object;
        }
    }
    return 0;
}

void SpriteCollisionManager::debug() {
	for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
		SpriteCollisionObject* object = m_Objects[i];
		LOG(logINFO) << i << " id: " << object->id << " entity-id: " << object->sprite->getID() << " type: " << object->type << " active: " << object->sprite->isActive() << " position: " << DBG_V2(object->sprite->getPosition()) << " radius: " << object->extent.x;
	}
}

namespace coll {

	bool containsType(const SpriteCollision& collision,int type) {
		return collision.firstType == type || collision.secondType == type;
	}

	int getIDByType(const SpriteCollision& collision,int type) {
		if ( collision.firstType == type ) {
			return collision.firstId;
		}
		return collision.secondId;
	}

	Vector2f getPositionByType(const SpriteCollision& collision,int type) {
		if ( collision.firstType == type ) {
			return collision.firstPosition;
		}
		return collision.secondPosition;
	}
}

}
