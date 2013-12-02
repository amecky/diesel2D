#pragma once
#include "Particle.h"
#include "ParticleBuffer.h"
#include <vector>
#include "..\utils\mtrand.h"
#include "..\utils\PlainTextReader.h"
#include "..\settings\DynamicSettings.h"

namespace ds {
// -------------------------------------------------------
// Particle emitter data
// -------------------------------------------------------
struct ParticleEmitterData : public DynamicSettings {

	uint32 ejectionPeriod;
	uint32 ejectionVariance;
	uint32 ejectionCounter;
	float velocity;
	float velocityVariance;
	float ttl;
	float ttlVariance;
	float sizeXVariance;
	float sizeYVariance;
	uint32 count;

	ParticleEmitterData() : ejectionPeriod(0) , ejectionVariance(0.0f) , ejectionCounter(0) , velocity(0.0f) , velocityVariance(0.0f) , ttl(1.0f) , ttlVariance(0.0f) , sizeXVariance(0.0f) , sizeYVariance(0.0f) , count(1) {}

	void load(NewSettingsReader& reader) {
		reader.get<float>("velocity",&velocity);
		reader.get<float>("velocity_variance",&velocityVariance);
		reader.get<uint32>("ejection_period",&ejectionPeriod);
		reader.get<uint32>("ejection_variance",&ejectionVariance);
		reader.get<uint32>("ejection_counter",&ejectionCounter);
		reader.get<float>("ttl",&ttl);
		reader.get<float>("ttl_variance",&ttlVariance);
		reader.get<float>("size_x_variance",&sizeXVariance);
		reader.get<float>("size_y_variance",&sizeYVariance);
		reader.get<uint32>("count",&count);
	}
};
// -------------------------------------------------------
// Particle emitter
// -------------------------------------------------------
class ParticleEmitter {

public:
	ParticleEmitter();
	ParticleEmitter(ParticleEmitterData* emitterData,ParticleData* particleData);
	ParticleEmitter(ParticleData* particleData);
	virtual ~ParticleEmitter();
	virtual void createParticles(ParticleBuffer* buffer,float forcedCount = -1) = 0;
	virtual void update(ParticleBuffer* buffer,float elapsed);
	void setPosition(const ds::Vec2& position) {
		m_Position = position;
	}
	const ds::Vec2& getPosition() const {
		return m_Position;
	}
	void emitParticles(ParticleBuffer* buffer);
	void setID(uint32 id) {
		m_ID = id;
	}
	const uint32 getID() const {
		return m_ID;
	}
	void resetCounter() {
		m_Counter = 0;
	}
protected:	
	Particle* prepare(ParticleBuffer* buffer);
	ds::Vec2 m_Position;
	ParticleEmitterData* m_Data;
	ParticleData* m_ParticleData;
private:
	uint32 m_ID;
	uint32 m_Timer;
	uint32 m_Next;
	uint32 m_Counter;
};

// -------------------------------------------------------
// 
// -------------------------------------------------------
class BlockEmitter : public ParticleEmitter {

public:
	BlockEmitter(ParticleEmitterData* emitterData,ParticleData* particleData) : ParticleEmitter(emitterData,particleData) {}
	virtual ~BlockEmitter() {}
	void createParticles(ParticleBuffer* buffer,float forcedCount = -1);
};

// -------------------------------------------------------
// 
// -------------------------------------------------------
class StarEmitter : public ParticleEmitter {

public:
	StarEmitter(ParticleEmitterData* emitterData,ParticleData* particleData) : ParticleEmitter(emitterData,particleData) {}
	virtual ~StarEmitter() {}
	void createParticles(ParticleBuffer* buffer,float forcedCount = -1);
};

// -------------------------------------------------------
// Ring emitter
// -------------------------------------------------------
struct RingEmitterSettings : public DynamicSettings {

	int radius;
	int count;
	int radiusVariance;
	float startAngle;
	float angleVariance;
	float startVariance;

	RingEmitterSettings() : radius(10) , count(10) , radiusVariance(2) , startAngle(0.0f) , angleVariance(0.0f) , startVariance(0.0f) {}

	void load(NewSettingsReader& reader) {
		reader.get<int>("radius",&radius);
		reader.get<int>("count",&count);
		reader.get<int>("radius_variance",&radiusVariance);
		reader.get<float>("start_angle",&startAngle);
		reader.get<float>("angle_variance",&angleVariance);
		reader.get<float>("start_variance",&startVariance);
	}

};
class RingEmitter : public ParticleEmitter {

public:
	RingEmitter() : ParticleEmitter() {}
	RingEmitter(ParticleEmitterData* emitterData,RingEmitterSettings* settings,ParticleData* particleData);
	virtual ~RingEmitter() {
		delete m_Settings;
	}
	void createParticles(ParticleBuffer* buffer,float forcedCount = -1);
	void setRadius(int radius) {
		m_Settings->radius = radius;
	}
private:
	RingEmitterSettings* m_Settings;
};

// -------------------------------------------------------
// Cone emitter settings
// -------------------------------------------------------
struct ConeEmitterSettings : public DynamicSettings {

	int radius;
	int count;
	int radiusVariance;
	float startAngle;
	float angleVariance;
	float endAngle;

	void load(NewSettingsReader& reader) {
		reader.get<int>("radius",&radius);
		reader.get<int>("count",&count);
		reader.get<int>("radius_variance",&radiusVariance);
		reader.get<float>("start_angle",&startAngle);
		reader.get<float>("angle_variance",&angleVariance);
		reader.get<float>("end_angle",&endAngle);
	}

};
// -------------------------------------------------------
// Cone emitter
// -------------------------------------------------------
class ConeEmitter : public ParticleEmitter {

public:	
	ConeEmitter(ParticleEmitterData* emitterData,ConeEmitterSettings* settings,ParticleData* particleData) 
		: ParticleEmitter(emitterData,particleData) , m_Settings(settings) {}
	virtual ~ConeEmitter() {
		delete m_Settings;
	}
	void createParticles(ParticleBuffer* buffer,float forcedCount = -1);	
	void setAngle(float startAngle,float endAngle) {
		m_Settings->startAngle = startAngle;
		m_Settings->endAngle = endAngle;
	}
private:
	ConeEmitterSettings* m_Settings;
};


}
