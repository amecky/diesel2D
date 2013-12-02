#include "CollisionManager.h"
#include "..\math\vector.h"
#include "World.h"

namespace ds {


CollisionManager::CollisionManager() : m_Counter(0) {
    m_ColBuffer = new char[COL_BUFFER_SIZE];
}

CollisionManager::CollisionManager(const CollisionManager& orig) {
}

CollisionManager::~CollisionManager() {
    delete m_ColBuffer;
}

bool CollisionManager::getCollision(int index,Collision& collision) const {
    if ( index < m_BufferCounter ) {
        char* buffer = m_ColBuffer + index * COL_SIZE;
        Collision col = *(Collision*)(buffer);
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

bool CollisionManager::bufferContains(int firstID, int secondID) {
    char* buffer = m_ColBuffer;
    for ( int i = 0; i < m_BufferCounter;++i) {
        Collision col = *(Collision*)(buffer);
        if ( col.firstId == firstID && col.secondId == secondID ) {
            return true;
        }
        if ( col.firstId == secondID && col.secondId == firstID ) {
            return true;
        }
        buffer += COL_SIZE;
    }
    return false;
}

void CollisionManager::reset() {
    m_Size = 0;
    m_BufferCounter = 0;
}

int CollisionManager::addLine(const Vec2& startPos, const Vec2& endPos) {
    int idx = m_Counter;
    CollisionObject co;
    co.prevPosition = startPos;
    //co.position = startPos;
    //co.offset = endPos;
    co.type = CDT_LINE;
    co.extent = Vec2(0.0f,0.0f);
    co.id = idx;
    m_Objects.push_back(co);
    ++m_Counter;
    return idx;
}

// -------------------------------------------------------
// Add circle
// -------------------------------------------------------
int CollisionManager::addCircle(Entity* entity,float radius,int type) {
    int idx = m_Counter;
    CollisionObject co;
    co.prevPosition = entity->getPosition();
    co.extent.x = radius;
    co.extent.y = 0.0f;
    co.shape = CDT_CIRCLE;
	co.type = type;
    co.id = idx;
	co.entity = entity;
    m_Objects.push_back(co);
    ++m_Counter;
    return idx;
}

// -------------------------------------------------------
// Add Box
// -------------------------------------------------------
int CollisionManager::addBox(Entity* entity,float width,float height,int type) {
	int idx = m_Counter;
	CollisionObject co;
	co.prevPosition = entity->getPosition();
	co.extent.x = width;
	co.extent.y = height;
	co.shape = CDT_BOX;
	co.type = type;
	co.id = idx;
	co.entity = entity;
	m_Objects.push_back(co);
	++m_Counter;
	return idx;
}

// -------------------------------------------------------
// Remove
// -------------------------------------------------------
void CollisionManager::remove(Entity* entity) {
	std::vector<CollisionObject>::iterator it = m_Objects.begin();
	while ( it != m_Objects.end() ) {
		if ( (*it).entity->getID() == entity->getID() ) {
			it = m_Objects.erase(it);
		}
		else {
			++it;
		}
	}
}

// -------------------------------------------------------
// Check intersections
// -------------------------------------------------------
void CollisionManager::checkIntersections() {
    for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
        CollisionObject* object = &m_Objects[i];
		if ( object->entity->isActive()) {
			for ( std::size_t j = 0; j < m_Objects.size(); ++j ) {
				CollisionObject* checkObject = &m_Objects[j];   
				if ( checkObject->entity->isActive()) {
					if ( !shouldIgnore(object->type,checkObject->type)) {
						if ( object->id != checkObject->id && object->type != checkObject->type ) {							
							if ( object->shape == CDT_BOX && checkObject->shape == CDT_CIRCLE )  {
								if ( checkBoxCircle(object->entity->getPosition(),object->extent.x,object->extent.y,checkObject->entity->getPosition(),checkObject->extent.x)) {
									addCollision(object,checkObject);
								}                    
							}
							else if ( object->shape == CDT_BOX )  {
								if ( checkBoxCircle(checkObject->entity->getPosition(),checkObject->extent.x,checkObject->extent.y,object->entity->getPosition(),object->extent.x)) {
									addCollision(object,checkObject);
								}                    
							}
							else if ( object->shape == CDT_CIRCLE && checkObject->shape == CDT_CIRCLE ) {
								//if ( circleSweepTest(object->id,checkObject->id)) {								
								if ( intersectCircleCircle(object,checkObject)) {
									addCollision(object,checkObject);
								}
							}
						}
					}				
					checkObject->prevPosition = object->entity->getPosition();
				}
			}
			object->prevPosition = object->entity->getPosition();
		}
    }
}

void CollisionManager::addCollision(CollisionObject* first, CollisionObject* second) {	
    if ( !bufferContains(first->entity->getID(),second->entity->getID()) ) {
		if ( m_Size < COL_BUFFER_SIZE ) {
			char* buffer = m_ColBuffer + m_Size;
			(*(Collision*)(buffer)).firstId = first->entity->getID();
			(*(Collision*)(buffer)).secondId = second->entity->getID();
			(*(Collision*)(buffer)).firstPosition = first->entity->getPosition();
			(*(Collision*)(buffer)).secondPosition = second->entity->getPosition();
			(*(Collision*)(buffer)).firstType = first->type;
			(*(Collision*)(buffer)).secondType = second->type;
			m_Size += COL_SIZE;
			++m_BufferCounter;
		}
    }    
}

bool CollisionManager::intersectCircleCircle(const CollisionObject* first, const CollisionObject* second) {
    Vec2 diff = first->entity->getPosition() - second->entity->getPosition();
    float distSq = vector::sqrLength(diff);
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

bool CollisionManager::intersectLineCircle(const Vec2& location, float radius, const Vec2& lineFrom, const Vec2& lineTo) {
    Vec2 ac = location - lineFrom;
    Vec2 ab = lineTo - lineFrom;
    float ab2 = vector::dot(ab,ab);
    float acab = vector::dot(ac,ab);
    float t = acab / ab2;

    if (t < 0)
        t = 0;
    else if (t > 1)
        t = 1;

    Vec2 h = ((ab * t) + lineFrom) - location;
    float h2 = vector::dot(h,h);

    return (h2 <= (radius * radius));
}
/*
void CollisionManager::updateCircle(int id, const Vec2& position) {
    for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
        CollisionObject* object = &m_Objects[i];
        if ( object->id == id && object->type == CDT_CIRCLE) {
            object->prevPosition = object->position;
            object->position = position;
        }
    }
}

void CollisionManager::updateLine(int id, const Vec2& startPos, const Vec2& endPos) {
    for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
        CollisionObject* object = &m_Objects[i];
        if ( object->id == id && object->type == CDT_LINE) {
            object->position = startPos;
            object->offset = endPos;
        }
    }
}
*/
bool CollisionManager::solveQuadraticFormula(float a, float b, float c, float* r1, float* r2) {
    float q = b * b - 4 * a * c;
    if( q >= 0.0f && a > 0.0f ) {
        float sq = sqrt(q);
        float d = 1 / (2*a);
        *r1 = ( -b + sq ) * d;
        *r2 = ( -b - sq ) * d;
        return true;//real roots
    }
    else {
        *r1 = 0.0f;
        *r2 = 0.0f;
        return false;//complex roots
    }
}

CollisionObject* CollisionManager::findObject(int id) {
    for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
        CollisionObject* object = &m_Objects[i];
        if ( object->id == id ) {
            return object;
        }
    }
    return 0;
}

bool CollisionManager::circleSweepTest(int firstID, int secondID) {
    CollisionObject* fc = findObject(firstID);
    CollisionObject* sc = findObject(secondID);
    float u0,u1;
    bool ret = sweepTest(fc->prevPosition,fc->entity->getPosition(),fc->extent.x,sc->prevPosition,sc->entity->getPosition(),sc->extent.x,&u0,&u1);
    // they will collide but not in this frame
    if ( u0 > 1.0f && u1 > 1.0f ) {
        ret = false;
    }
    return ret;
}

bool CollisionManager::sweepTest(const Vec2& a0, const Vec2& a1, float ra, const Vec2& b0, const Vec2& b1, float rb, float* u0, float* u1) {
    //vector from A0 to A1
    Vec2 va = a1 - a0;    
    //vector from B0 to B1
    Vec2 vb = b1 - b0;
    //vector from A0 to B0
    Vec2 AB = b0 - a0;
    Vec2 AB1 = b1 - a1;
    Vec2 vab = vb - va;
    //relative velocity (in normalized time)
    float rab = ra + rb;
    float a = vector::dot(vab,vab);
    //u*u coefficient
    float b = 2 * vector::dot(vab,AB);
    //u coefficient
    float c = vector::dot(AB,AB) - rab * rab;
    //constant term
    //check if they're currently overlapping
    if( vector::dot(AB,AB) <= rab * rab ) {
        *u0 = 0;
        *u1 = 0;
        return true;
    }
    if( vector::dot(AB1,AB1) <= rab * rab ) {
        *u0 = 0;
        *u1 = 0;
        return true;
    }
    //check if they hit each other 
    // during the frame
    if( solveQuadraticFormula( a, b, c, u0, u1 ) ) {        
        if( u0 > u1 ) {
            float tmp = *u0;
            *u0 = *u1;
            *u1 = tmp;
        }
        return true;
    }
    return false;
}

// ---------------------------------------------------------------
// Box -> Circle
// ---------------------------------------------------------------
bool CollisionManager::checkBoxCircle(const Vec2& p1,float w,float h,const Vec2& pos,float radius) const {
	float halfWidth = w * 0.5f;
	float halfHeight = h * 0.5f;
	Vec2 correctPos = p1;
	correctPos.x = p1.x - halfWidth;
	correctPos.y = p1.y - halfHeight;
	float cx = abs(pos.x - correctPos.x - halfWidth);
	float xDist = halfWidth + radius;
	if (cx > xDist) {
		return false;
	}
	float cy = abs(pos.y - correctPos.y - halfHeight);
	float yDist = halfHeight + radius;
	if (cy > yDist) {
		return false;
	}
	if (cx <= halfWidth || cy <= halfHeight) {
		return true;
	}
	float xCornerDist = cx - halfWidth;
	float yCornerDist = cy - halfHeight;
	float xCornerDistSq = xCornerDist * xCornerDist;
	float yCornerDistSq = yCornerDist * yCornerDist;
	float maxCornerDistSq = radius * radius;
	return xCornerDistSq + yCornerDistSq <= maxCornerDistSq;	
}

void CollisionManager::debug() {
	char buffer[256];
	for ( std::size_t i = 0; i < m_Objects.size(); ++i ) {
		CollisionObject* object = &m_Objects[i];
		sprintf(buffer,"%d id: %3d entity-id: %3d shape: %d type: %2d entity-type: %d active: %d",i,object->id,object->entity->getID(),object->shape,object->type,object->entity->getType(),object->entity->isActive());
		LOG(logINFO) << buffer;
	}
}

namespace coll {

	bool containsType(const Collision& collision,int type) {
		return collision.firstType == type || collision.secondType == type;
	}

	int getIDByType(const Collision& collision,int type) {
		if ( collision.firstType == type ) {
			return collision.firstId;
		}
		return collision.secondId;
	}

	Vec2 getPositionByType(const Collision& collision,int type) {
		if ( collision.firstType == type ) {
			return collision.firstPosition;
		}
		return collision.secondPosition;
	}
}


}
