/* 
 * File:   NarrowPhaseDetector.h
 * Author: meckya
 *
 * Created on 15. Mai 2014, 15:27
 */

#ifndef NARROWPHASEDETECTOR_H
#define	NARROWPHASEDETECTOR_H
#include "..\lib\DataArray.h"
#include "CollisionTypes.h"

namespace ds {

class NarrowPhaseDetector {
    
public:
    NarrowPhaseDetector();
    virtual ~NarrowPhaseDetector();
    uint32 detect(CollisionContext& context);    
private:
    bool intersectCircleCircle(const Collider& first, const Collider& second);
    bool sweepTest(const Vector2f& a0, const Vector2f& a1, float ra, const Vector2f& b0, const Vector2f& b1, float rb, float* u0, float* u1);
    bool solveQuadraticFormula(float a, float b, float c, float* r1, float* r2);
    NarrowPhaseDetector(const NarrowPhaseDetector& orig);

};

}

#endif	/* NARROWPHASEDETECTOR_H */

