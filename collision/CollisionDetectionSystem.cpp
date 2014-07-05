/* 
 * File:   CollisionDetectionSystem.cpp
 * Author: meckya
 * 
 * Created on 14. Mai 2014, 08:22
 */

#include "CollisionDetectionSystem.h"
#include "BroadPhaseDetector.h"
#include "SpatialHashingBroadPhaseDetector.h"
#include <stdio.h>
#include <vector>
#include <math.h>

namespace ds {

CollisionDetectionSystem::CollisionDetectionSystem() {
    //broadPhaseDetector = new BruteForceBroadPhaseDetector();
    broadPhaseDetector = new SpatialHashingBroadPhaseDetector();
}

CollisionDetectionSystem::CollisionDetectionSystem(const CollisionDetectionSystem& orig) {
}

CollisionDetectionSystem::~CollisionDetectionSystem() {
    delete broadPhaseDetector;
}

// ---------------------------------------------------------
// Add circle collider
// ---------------------------------------------------------
ID CollisionDetectionSystem::addCircle(const Vector2f& pos, float radius, uint32 type) {
    return addCollider(CT_CIRCLE,pos,Vector2f(radius,0.0f),type);
}

// ---------------------------------------------------------
// Add collider
// ---------------------------------------------------------
ID CollisionDetectionSystem::addCollider(ColliderType colliderType, const Vector2f& pos, const Vector2f& extent,uint32 type) {
    ID id = m_Context.colliders.add();
    Collider& c = m_Context.colliders.get(id);
    c.type = type;
    c.position = pos;
    c.extent = extent;
    c.colliderType = colliderType;
    return id;
}

// ---------------------------------------------------------
// Update
// ---------------------------------------------------------
void CollisionDetectionSystem::update(const ID& id, const Vector2f& pos) {
    Collider& c = m_Context.colliders.get(id);
    c.prevPosition = c.position;
    c.position = pos;
}
// ---------------------------------------------------------
// Reset
// ---------------------------------------------------------
void CollisionDetectionSystem::reset() {
    m_Context.reset();
}

// ---------------------------------------------------------
// check intersection
// ---------------------------------------------------------
uint32 CollisionDetectionSystem::checkIntersections() {
    
    broadPhaseDetector->detect(m_Context);
    printf("broad phase collisions %d\n",m_Context.numBroadPhaseCollisions);
    m_Context.numNarrowPhaseCollisions = narrowPhaseDetector.detect(m_Context);
    printf("narrow %d\n",m_Context.numNarrowPhaseCollisions);
    for ( uint32 i = 0; i < m_Context.numNarrowPhaseCollisions; ++i ) {
        CollisionPair& pair = m_Context.narrowPhaseCollisions[i];
        Collider& c1 = m_Context.colliders.get(pair.first);
        Collider& c2 = m_Context.colliders.get(pair.second);
        m_Context.addCollision(c1,c2);
    }
	return m_Context.colBufferIndex;
}

}