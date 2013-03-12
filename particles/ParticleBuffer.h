#pragma once
#include "..\utils\QuadBuffer.h"
#include "Particle.h"
#include "..\renderer\Renderer.h"

namespace ds {

class ParticleBuffer : public QuadBuffer {

public:
	ParticleBuffer(Renderer* renderer,uint32 maxParticles);
	virtual ~ParticleBuffer() {}
	Particle* freeParticle(Particle* particle);	
	void manageLifecyle(float elapsed);
	void moveParticles(float elapsed);
	uint32 size();
	Particle* createParticle(ParticleData* data);
	Particle* getParticles();
private:
	int findParticle(Particle* particle);
	Particle* m_ParticleHead;
	Particle* m_FreeList;
};

}
