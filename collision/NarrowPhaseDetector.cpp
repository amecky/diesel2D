/* 
 * File:   NarrowPhaseDetector.cpp
 * Author: meckya
 * 
 * Created on 15. Mai 2014, 15:27
 */

#include "NarrowPhaseDetector.h"
#include <stdio.h>
#include <math.h>

namespace ds {

NarrowPhaseDetector::NarrowPhaseDetector() {
}

NarrowPhaseDetector::NarrowPhaseDetector(const NarrowPhaseDetector& orig) {
}

NarrowPhaseDetector::~NarrowPhaseDetector() {
}

uint32 NarrowPhaseDetector::detect(CollisionContext& context) {
    for ( uint32 i = 0; i < context.numBroadPhaseCollisions; ++i ) {
        CollisionPair& pair = context.broadPhaseCollisions[i];
        Collider& first = context.colliders.get(pair.first);
        Collider& second = context.colliders.get(pair.second);
        if ( first.colliderType == CT_CIRCLE && second.colliderType == CT_CIRCLE ) {
            if ( intersectCircleCircle(first,second)) {
                context.narrowPhaseCollisions[context.numNarrowPhaseCollisions++] = CollisionPair(first.id,second.id);
            }
        }
    }
    return context.numNarrowPhaseCollisions;
}

// ---------------------------------------------------------
// circle to circle intersection
// ---------------------------------------------------------
bool NarrowPhaseDetector::intersectCircleCircle(const Collider& first, const Collider& second) {
    float u0 = 0.0f;
    float u1 = 0.0f;
    return sweepTest(first.prevPosition,first.position,first.extent.x,second.prevPosition,second.position,second.extent.x,&u0,&u1);
}

// ---------------------------------------------------------
// Circle sweep test
// ---------------------------------------------------------
bool NarrowPhaseDetector::sweepTest(const Vector2f& a0, const Vector2f& a1, float ra, const Vector2f& b0, const Vector2f& b1, float rb, float* u0, float* u1) {
    //vector from A0 to A1
    Vector2f va = a1 - a0;    
    //vector from B0 to B1
    Vector2f vb = b1 - b0;
    //vector from A0 to B0
    Vector2f AB = b0 - a0;
    Vector2f AB1 = b1 - a1;
    Vector2f vab = vb - va;
    //relative velocity (in normalized time)
    float rab = ra + rb;
    float a = dot(vab,vab);
    //u*u coefficient
    float b = 2 * dot(vab,AB);
    //u coefficient
    float c = dot(AB,AB) - rab * rab;
    //constant term
    //check if they're currently overlapping
    if( dot(AB,AB) <= rab * rab ) {
        *u0 = 0;
        *u1 = 0;
        return true;
    }
    if( dot(AB1,AB1) <= rab * rab ) {
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

// ---------------------------------------------------------
// solve quadratic formula
// ---------------------------------------------------------
bool NarrowPhaseDetector::solveQuadraticFormula(float a, float b, float c, float* r1, float* r2) {
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

}