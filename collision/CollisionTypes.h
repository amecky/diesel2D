/* 
 * File:   CollisionTypes.h
 * Author: meckya
 *
 * Created on 15. Mai 2014, 15:24
 */

#ifndef COLLISIONTYPES_H
#define	COLLISIONTYPES_H
#include "..\dxstdafx.h"
#include "..\lib\DataArray.h"
#include "..\..\math\Vector.h"

namespace ds {
// ---------------------------------------------------------
// AABBox
// ---------------------------------------------------------
struct AABBox {
    
    float top;
    float left;
    float right;
    float bottom;
    
    AABBox() : top(0.0f) , left(0.0f) , right(0.0f) , bottom(0.0f) {}
    
    AABBox(const Vector2f& center,float extent) {
        top = center.y - extent;
        left = center.x - extent;
        right = center.x + extent;
        bottom = center.y + extent;
    }
    
    bool intersects(const AABBox& other) {
        if( right < other.left || left > other.right ) return false;
        if( bottom < other.top || top > other.bottom ) return false;
        return true;
    }
};

// ---------------------------------------------------------
// collider types
// ---------------------------------------------------------
enum ColliderType {CT_BOX,CT_CIRCLE};

// ---------------------------------------------------------
// collider
// ---------------------------------------------------------
struct Collider {
    
    ID id;
    ColliderType colliderType;
    Vector2f position;
    Vector2f prevPosition;
    uint32 type;
    Vector2f extent;
};

// ---------------------------------------------------------
// collision 
// ---------------------------------------------------------
struct Collision {
    
    ID firstID;
    ID secondID;
    Vector2f firstPosition;
    Vector2f secondPosition;
    uint32 firstType;
    uint32 secondType;
    
    Vector2f firstContact;
    Vector2f secondContact;
    
};

// ---------------------------------------------------------
// Collision pair
// ---------------------------------------------------------
struct CollisionPair {
    
    ID first;
    ID second;
    
    CollisionPair() : first(0) , second(0) {}
    CollisionPair(ID first,ID second) : first(first) , second(second) {}
    
    bool equals(ID firstID,ID secondID) {
        if ( first == firstID && second == secondID ) {
            return true;
        }
        if ( first == secondID && second == firstID ) {
            return true;
        }
        return false;
    }
    
    
};

// ---------------------------------------------------------
// Collision context
// ---------------------------------------------------------
struct CollisionContext {
    
    DataArray<Collider> colliders;
    uint32 numCollisions;
    uint32 numBroadPhaseCollisions;
    uint32 numNarrowPhaseCollisions;
    CollisionPair broadPhaseCollisions[256];
    CollisionPair narrowPhaseCollisions[256];
    Collision collisionBuffer[128];
    uint32 colBufferIndex;
    
    void reset() {
        numBroadPhaseCollisions = 0;
        numNarrowPhaseCollisions = 0;
        numCollisions = 0;
        colBufferIndex = 0;
        for ( uint32 i = 0; i < colliders.numObjects; ++i ) {
            Collider& c = colliders.objects[i];
            c.prevPosition = c.position;
        }
    }
    
    Collider& getCollider(ID id) {
        return colliders.get(id);
    }
    
    void getCollision(uint32 index, Collision* collision) {
        Collision col = collisionBuffer[index];
        collision->firstID = col.firstID;
        collision->secondID = col.secondID;
        collision->firstPosition = col.firstPosition;
        collision->secondPosition = col.secondPosition;
        collision->firstType = col.firstType;
        collision->secondType = col.secondType;
    }

        
    void addCollision(const Collider& first,const Collider& second) {
        Collision col;
        col.firstID = first.id;
        col.firstPosition = first.position;
        col.firstType = first.type;
        col.secondID = second.id;
        col.secondPosition = second.position;
        col.secondType = second.type;
        collisionBuffer[colBufferIndex++] = col;
    }
    
    void addBroadPhaseCollision(ID first,ID second) {
        bool found = false;
        for ( uint32 i = 0; i < numBroadPhaseCollisions; ++i ) {
            CollisionPair& pair = broadPhaseCollisions[i];
            if ( pair.equals(first,second)) {
                found = true;
            }
        }
        if ( !found ) {
            CollisionPair cp(first,second);
            broadPhaseCollisions[numBroadPhaseCollisions++] = cp;
        }
    }
};

}

#endif	/* COLLISIONTYPES_H */

