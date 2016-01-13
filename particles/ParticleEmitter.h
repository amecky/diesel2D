#pragma once
#include "Particle.h"
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

	ParticleEmitterData() : ejectionPeriod(0), ejectionVariance(0), ejectionCounter(0), count(1) , duration(0.0f) {}

};

struct ParticleEmitterInstance {

	ID id;
	v3 pos;
	float timer;
	float accumulated;
	float ttl;

};

typedef DataArray<ParticleEmitterInstance, 64> EmitterInstances;

}


