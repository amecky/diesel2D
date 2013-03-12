#pragma once
#include "Particle.h"
#include "ParticleBuffer.h"
#include <vector>
#include "..\utils\mtrand.h"

namespace ds {
// -------------------------------------------------------
// Particle emitter data
// -------------------------------------------------------
class ParticleEmitterData {

public:
	ParticleEmitterData() {
		ejectionPeriod = 0;
		ejectionVariance = 0;
		velocity = 0.0f;
		velocityVariance = 0.0f;
	}
	~ParticleEmitterData();
	uint32 ejectionPeriod;
	uint32 ejectionVariance;
	float velocity;
	float velocityVariance;
};
// -------------------------------------------------------
// Particle emitter
// -------------------------------------------------------
class ParticleEmitter {

typedef std::vector<ParticleData*> ParticleDataList;

public:
	ParticleEmitter(ParticleEmitterData* emitterData);
	virtual ~ParticleEmitter();
	virtual void createParticles(ParticleBuffer* buffer,ParticleData* particleData) = 0;
	virtual void update(ParticleBuffer* buffer,float elapsed);
	void setPosition(const ds::Vec2& position) {
		m_Position = position;
	}
	const ds::Vec2& getPosition() const {
		return m_Position;
	}
	void addParticleData(ParticleData* particleData);
	void emitParticles(ParticleBuffer* buffer);
	void setID(uint32 id) {
		m_ID = id;
	}
	const uint32 getID() const {
		return m_ID;
	}
protected:
	float random(float min,float max) {
		return min + (max - min)* (float)rand();
	}	
	ds::Vec2 m_Position;
	ParticleEmitterData* m_Data;
private:
	uint32 m_ID;
	uint32 m_Timer;
	uint32 m_Next;
	ParticleDataList m_ParticleDataList;
	MTRand_open rand;
};

// -------------------------------------------------------
// 
// -------------------------------------------------------
class BlockEmitter : public ParticleEmitter {

public:
	BlockEmitter(ParticleEmitterData* emitterData) : ParticleEmitter(emitterData) {}
	virtual ~BlockEmitter() {}
	void createParticles(ParticleBuffer* buffer,ParticleData* particleData);
};

// -------------------------------------------------------
// 
// -------------------------------------------------------
class StarEmitter : public ParticleEmitter {

public:
	StarEmitter(ParticleEmitterData* emitterData) : ParticleEmitter(emitterData) {}
	virtual ~StarEmitter() {}
	void createParticles(ParticleBuffer* buffer,ParticleData* particleData);
};

}
