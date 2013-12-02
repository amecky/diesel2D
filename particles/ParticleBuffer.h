#pragma once
#include "Particle.h"
#include "..\memory\FSAllocator.h"

namespace ds {

class ParticleBuffer {

public:
	ParticleBuffer(uint32 maxParticles);
	virtual ~ParticleBuffer() {}
	void freeParticle(Particle* particle);	
	void manageLifecyle(float elapsed);
	uint32 size();
	uint32 free();
	Particle* createParticle(ParticleData* data);
	Particle* getParticles();
	const uint32 getMaxParticles() const {
		return m_MaxParticles;
	}
private:
	Particle* m_Head;
	Particle* m_FreeList;
	uint32 m_MaxParticles;
	uint32 m_Counter;
	FSAllocator<Particle> m_Allocator;
};

}
