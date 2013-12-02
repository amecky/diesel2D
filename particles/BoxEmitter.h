#pragma once
#include "ParticleEmitter.h"

namespace ds {

// -------------------------------------------------------
// BoxEmitterSettings
// -------------------------------------------------------
struct BoxEmitterSettings : public DynamicSettings {

	int width;
	int height;
	int size;	
	float angleVariance;

	void load(NewSettingsReader& reader) {
		reader.get<int>("width",&width);
		reader.get<int>("height",&height);
		reader.get<int>("size",&size);
		reader.get<float>("angle_variance",&angleVariance);
	}

};

class BoxEmitter : public ParticleEmitter {

public:
	BoxEmitter(ParticleEmitterData* emitterData,BoxEmitterSettings* settings,ParticleData* particleData) 
		: ParticleEmitter(emitterData,particleData) , m_Settings(settings) {}
	virtual ~BoxEmitter() {
		delete m_Settings;
	}
	void createParticles(ParticleBuffer* buffer,float forcedCount = -1);	
private:
	BoxEmitterSettings* m_Settings;
};

}

