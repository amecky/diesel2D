/* 
 * File:   BroadPhaseDetector.cpp
 * Author: meckya
 * 
 * Created on 15. Mai 2014, 09:03
 */

#include "BroadPhaseDetector.h"
#include <math.h>

namespace ds {

AABBox BroadPhaseDetector::buildAABBox(const Collider& collider) {
    AABBox box;
    float ex = collider.extent.x;
    box.left = min(collider.position.x - ex,collider.prevPosition.x - ex);
    box.top = min(collider.position.y - ex,collider.prevPosition.y - ex);
    box.right = max(collider.position.x + ex,collider.prevPosition.x + ex);
    box.bottom = max(collider.position.y + ex,collider.prevPosition.y + ex);
    return box;
}

AABBox BroadPhaseDetector::buildAABBox(const Vector2f& center,float extent) {
    AABBox box;
    box.left = center.x - extent;
    box.top = center.y - extent;
    box.right = center.x + extent;
    box.bottom = center.y + extent;
    return box;
}

// ---------------------------------------------------------
// check intersection
// ---------------------------------------------------------
void BruteForceBroadPhaseDetector::detect(CollisionContext& context) {
    for ( uint32 i = 0; i < context.colliders.numObjects; ++i ) {
        Collider& c = context.colliders.objects[i];
        for ( uint32 j = 0; j < context.colliders.numObjects; ++j ) {
            Collider& other = context.colliders.objects[j];
            if ( c.id != other.id ) {
                if ( intersectBoxes(c,other)) {
                    context.addBroadPhaseCollision(c.id,other.id);
                }
            }
        }
    }
}



// ---------------------------------------------------------
// Interseect boxes
// ---------------------------------------------------------
bool BruteForceBroadPhaseDetector::intersectBoxes(const Collider& first, const Collider& second) {
    // AABB 1
    AABBox firstBox = buildAABBox(first);
    // AABB 2
    AABBox secondBox = buildAABBox(second);
    return firstBox.intersects(secondBox);
}

}