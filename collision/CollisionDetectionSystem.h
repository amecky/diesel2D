/* 
 * File:   CollisionDetectionSystem.h
 * Author: meckya
 *
 * Created on 14. Mai 2014, 08:22
 */

#ifndef COLLISIONDETECTIONSYSTEM_H
#define	COLLISIONDETECTIONSYSTEM_H

#include "..\lib\DataArray.h"
#include <vector>
#include "CollisionTypes.h"
#include "BroadPhaseDetector.h"
#include "NarrowPhaseDetector.h"

namespace ds {
// ---------------------------------------------------------
// collision detector
// ---------------------------------------------------------
class CollisionDetectionSystem {
    
public:
    CollisionDetectionSystem();
    
    virtual ~CollisionDetectionSystem();
    ID addCircle(const Vector2f& pos,float radius,uint32 type);
    void reset();
    const uint32 getNumCollisions() const {
        return m_Context.colBufferIndex;
    }
    uint32 checkIntersections();
    void update(const ID& id,const Vector2f& pos);
    void getCollision(uint32 index,Collision* collision) {
        m_Context.getCollision(index,collision);
    }
private:
    ID addCollider(ColliderType colliderType,const Vector2f& pos,const Vector2f& extent,uint32 type);
    CollisionContext m_Context;
    BroadPhaseDetector* broadPhaseDetector;
    NarrowPhaseDetector narrowPhaseDetector;
    CollisionDetectionSystem(const CollisionDetectionSystem& orig);
};

}

#endif	/* COLLISIONDETECTIONSYSTEM_H */

