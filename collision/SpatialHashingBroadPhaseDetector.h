/* 
 * File:   SpatialHashingBroadPhaseDetector.h
 * Author: meckya
 * http://www.cs.ucf.edu/~jmesit/publications/scsc%202005.pdf
 * http://gameprogrammingpatterns.com/spatial-partition.html
 * Created on 21. Mai 2014, 09:35
 */

#ifndef SPATIALHASHINGBROADPHASEDETECTOR_H
#define	SPATIALHASHINGBROADPHASEDETECTOR_H
#include "BroadPhaseDetector.h"
#include <vector>
#include <map>

const int GRID_SIZE = 50;
const float HALF_GRID_SIZE = 25.0f;
const int GRID_DIM = 20;
const int GRID_ITEMS = GRID_DIM * GRID_DIM;

namespace ds {
// -------------------------------------------------------
// 
// -------------------------------------------------------
class SpatialHashingBroadPhaseDetector : public BroadPhaseDetector {

struct Bucket {
    uint32 index;
    AABBox box;
    std::vector<ID> entries;
};    

public:
    SpatialHashingBroadPhaseDetector();
    virtual ~SpatialHashingBroadPhaseDetector() {}
    void detect(CollisionContext& context);
    void debug();
private:
    uint32 convertToGridIndex(const Vector2f& pos);
    void addToBucket(const Vector2i& point,const Collider& c);
    void checkIntersection(CollisionContext& context,const Vector2f& pos,Collider& c);
    bool check(const Vector2i& point,const AABBox& box);
    Bucket m_Buckets[GRID_DIM * GRID_DIM];
    Vector2i convertToGrid(const Vector2f& position);
};

}

#endif	/* SPATIALHASHINGBROADPHASEDETECTOR_H */

