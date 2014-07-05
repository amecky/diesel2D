/* 
 * File:   SpatialHashingBroadPhaseDetector.cpp
 * Author: meckya
 * 
 * Created on 21. Mai 2014, 09:35
 */

#include <vector>
#include <stdio.h>
#include "SpatialHashingBroadPhaseDetector.h"
#include "CollisionTypes.h"

const int X_P[] = {-1,0,1};

namespace ds {
// --------------------------------------------------------
// Constructor
// --------------------------------------------------------
SpatialHashingBroadPhaseDetector::SpatialHashingBroadPhaseDetector() : BroadPhaseDetector() {
    for ( uint32 x = 0; x < GRID_DIM; ++x ) {
        for ( uint32 y = 0; y < GRID_DIM; ++y ) {
            Bucket& b = m_Buckets[x + y * GRID_DIM];
            b.index = x + y * GRID_DIM;
            float xp = x * GRID_SIZE + HALF_GRID_SIZE;
            float yp = y * GRID_SIZE + HALF_GRID_SIZE;
            b.box = buildAABBox(Vector2f(xp,yp),HALF_GRID_SIZE);
        }
    }
}

// --------------------------------------------------------
// Convert to grid
// --------------------------------------------------------
Vector2i SpatialHashingBroadPhaseDetector::convertToGrid(const Vector2f& position) {
    int x = static_cast<int>(position.x) / GRID_SIZE;
    int y = static_cast<int>(position.y) / GRID_SIZE;
    return Vector2i(x,y);
}
// --------------------------------------------------------
// Detect
// --------------------------------------------------------
void SpatialHashingBroadPhaseDetector::detect(CollisionContext& context) {
    // clear buckets
    for ( uint32 i = 0; i < GRID_ITEMS;++i ) {
        m_Buckets[i].entries.clear();
    }
    // rebuild buckets
    for ( uint32 i = 0; i < context.colliders.numObjects; ++i ) {
        Collider& c = context.colliders.objects[i];        
        Vector2i current = convertToGrid(c.position);
        Vector2i prev = convertToGrid(c.prevPosition);
        int ex = current.x;
        int ey = current.y;
        int sx = prev.x;
        int sy = prev.y;
        if ( ex < sx ) {
            int t = sx;
            sx = ex;
            ex = t;
        }
        if ( ey < sy ) {
            int t = sy;
            sy = ey;
            ey = t;
        }
        int dx = ex - sx;
        int dy = ey - sy;
        if ( dx == 0 && dy == 0 ) {
            // not moved
            addToBucket(current,c);
        }
        else {
            for ( int x = sx ; x <= ex; ++x ) {
                for ( int y = sy; y <= ey; ++y ) {
                    addToBucket(Vector2i(x,y),c);        
                }
            }
        }
    }
    // check intersections
    for ( uint32 i = 0; i < context.colliders.numObjects; ++i ) {
        Collider& c = context.colliders.objects[i];
        checkIntersection(context,c.position,c);
    }
}

// --------------------------------------------------------
// Add to bucket
// --------------------------------------------------------
void SpatialHashingBroadPhaseDetector::addToBucket(const Vector2i& point, const Collider& c) {
    int idx = point.x + point.y * GRID_DIM;
    Bucket& b = m_Buckets[idx];
    bool found = false;
    for ( int i = 0; i < b.entries.size(); ++i ) {
        if ( b.entries[i] == c.id ) {
            found = true;
        }
    }
    if ( !found ) {
        b.entries.push_back(c.id);
    }
}

// --------------------------------------------------------
// Check 
// --------------------------------------------------------
bool SpatialHashingBroadPhaseDetector::check(const Vector2i& point, const AABBox& box) {
    int idx = point.x + point.y * GRID_DIM;
    Bucket& b = m_Buckets[idx];
    if ( b.box.intersects(box)) {
        return true;
    }
    return false;
}

// --------------------------------------------------------
// Check interscetions
// --------------------------------------------------------
void SpatialHashingBroadPhaseDetector::checkIntersection(CollisionContext& context,const Vector2f& pos,Collider& c) {    
    AABBox box = buildAABBox(c);
    Vector2i gp = convertToGrid(c.position);
    for ( int gx = 0; gx < 3; ++gx ) {
        for ( int gy = 0; gy < 3; ++gy ) {
            Vector2i checkPoint(gp.x + X_P[gx],gp.y + X_P[gy]);
            int idx = checkPoint.x + checkPoint.y * GRID_DIM;
            Bucket& b = m_Buckets[idx];
            for ( int i = 0; i < b.entries.size(); ++i ) {
                ID id = b.entries[i];
                if ( id != c.id ) {
                    Collider& other = context.colliders.get(id);
                    AABBox otherBox = buildAABBox(other);
                    if ( box.intersects(otherBox) ) {
                        context.addBroadPhaseCollision(c.id,other.id);
                    }
                }
            }
        }
    }
}

// --------------------------------------------------------
// Convert vec2 to grid position
// --------------------------------------------------------
uint32 SpatialHashingBroadPhaseDetector::convertToGridIndex(const Vector2f& pos) {
    int x = static_cast<int>(pos.x) / GRID_SIZE;
    int y = static_cast<int>(pos.y) / GRID_SIZE;
    int ret = x + y * GRID_DIM;
    return ret;
}

void SpatialHashingBroadPhaseDetector::debug() {
    for ( uint32 i = 0; i < GRID_ITEMS; ++i ) {
        Bucket& b = m_Buckets[i];
        if ( !b.entries.empty() ) {
            printf("Index %d : ",b.index);
            for ( int j = 0; j < b.entries.size(); ++j ) {
                printf("%d ",b.entries[j]);
            }
            printf("\n");
        }
    }
}

}