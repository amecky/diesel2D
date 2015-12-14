#pragma once
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryLoader.h"
#include "Particle.h"
#include "..\io\BinaryWriter.h"
#include "ParticleGenerator.h"

namespace ds {

// -------------------------------------------------------
// Particle emitter data
// -------------------------------------------------------
struct ParticleEmitterData {

	uint32 ejectionPeriod;
	uint32 ejectionVariance;
	uint32 ejectionCounter;
	uint32 count;
	float duration; // seconds
	int loops; // how many ticks to run
	bool burst;
	bool endless;
	float frequency;

	ParticleEmitterData() : ejectionPeriod(0), ejectionVariance(0), ejectionCounter(0), count(1) {}

	void load(BinaryLoader* loader) {
		loader->read(&count);
		loader->read(&ejectionPeriod);
		loader->read(&ejectionVariance);
		loader->read(&ejectionCounter);
		loader->read(&duration);
	}
};

struct ParticleEmitterInstance {

	ID id;
	v3 pos;
	float timer;
	float accumulated;
	float ttl;

};

typedef DataArray<ParticleEmitterInstance, 64> EmitterInstances;
/*
class ParticleEmitter {

public:
	ParticleEmitter();
	~ParticleEmitter();
	//void clear(bool addDefault = true);
	//void start();
	//void generate(ParticleArray* array, const v3& pos, float dt, uint32* start, uint32* end, bool started = true);
	//ParticleEmitterData& getEmitterData();
	//void tick(float dt);
	//void init();
	//void stop();
private:
	void generate(ParticleArray* array, const v3& pos, int count, uint32* start, uint32* end, float dt);
	ParticleEmitterData m_EmitterData;
	//float m_Duration;
	float m_Timer;
	bool m_Active;
	//bool m_Burst;
	//bool m_Endless;
	float m_Accu;
	//float m_Frequency;
};
*/

}


