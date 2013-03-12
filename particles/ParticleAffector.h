#pragma once
#include "Particle.h"

namespace ds {
// -------------------------------------------------------
// ParticleAffector virtual base class 
// -------------------------------------------------------
class ParticleAffector {

public:
	ParticleAffector() {}
	virtual ~ParticleAffector() {}
	virtual void update(Particle* particle,float elapsed) = 0;
};

// -------------------------------------------------------
// Size affector
// -------------------------------------------------------
class SizeAffector : public ParticleAffector {

public:
	SizeAffector(float minX,float maxX,float minY,float maxY);
	void update(Particle* particle,float elapsed);
private:
	float m_MinX;
	float m_MaxX;
	float m_MinY;
	float m_MaxY;
};
// -------------------------------------------------------
//  Color affector
// -------------------------------------------------------
class ColorAffector : public ParticleAffector {

public:
	void update(Particle* particle,float elapsed);
};

// -------------------------------------------------------
// Wiggle Affector
// -------------------------------------------------------
class WiggleAffector : public ParticleAffector {

public:
	WiggleAffector(float radius,float amplitude) : ParticleAffector() , m_Radius(radius) , m_Ampitude(amplitude) {}
	void update(Particle* particle,float elapsed);
private:
	float m_Radius;
	float m_Ampitude;

};

}
