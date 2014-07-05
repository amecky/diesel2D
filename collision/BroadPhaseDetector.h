/* 
 * File:   BroadPhaseDetector.h
 * Author: meckya
 *
 * Created on 15. Mai 2014, 09:03
 */

#ifndef BROADPHASEDETECTOR_H
#define	BROADPHASEDETECTOR_H
#include "..\lib\DataArray.h"
#include "CollisionTypes.h"

namespace ds {
// -------------------------------------------------------
// Broadphase detector virtual class
// -------------------------------------------------------
class BroadPhaseDetector {
    
public:
    BroadPhaseDetector() {}
    virtual ~BroadPhaseDetector() {}
    virtual void detect(CollisionContext& context) = 0;
protected:
    AABBox buildAABBox(const Collider& collider);
    AABBox buildAABBox(const Vector2f& center,float extent);
private:
    BroadPhaseDetector(const BroadPhaseDetector& orig) {}
};

// -------------------------------------------------------
// brute force broadphase detector
// -------------------------------------------------------
class BruteForceBroadPhaseDetector : public BroadPhaseDetector {
    
public:
    BruteForceBroadPhaseDetector() : BroadPhaseDetector() {}
    virtual ~BruteForceBroadPhaseDetector() {}
    void detect(CollisionContext& context);
private:
    bool intersectBoxes(const Collider& first,const Collider& second);
    
};

}

#endif	/* BROADPHASEDETECTOR_H */

