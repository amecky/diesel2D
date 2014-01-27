#pragma once
#include "ParticleEmitter.h"
#include "..\data\Gizmo.h"

namespace ds {

// -------------------------------------------------------
// BoxEmitterSettings
// -------------------------------------------------------
struct BoxEmitterSettings : public Gizmo {

	int width;
	int height;
	int size;	
	float angleVariance;

	BoxEmitterSettings() : Gizmo("box_emitter") , width(10) , height(10) , size(2) , angleVariance(0.0f) {
		add("width",&width);
		add("height",&height);
		add("size",&size);
		add("angle_variance",&angleVariance);	
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

