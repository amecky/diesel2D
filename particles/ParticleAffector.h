#pragma once
#include "Particle.h"
#include "..\math\FloatArray.h"
#include "..\dxstdafx.h"
#include "..\pipeline\PAKWriter.h"
#include "..\pipeline\PAKReader.h"
#include "..\utils\PlainTextReader.h"
#include "..\renderer\Renderer.h"
#include "..\settings\DynamicSettings.h"

namespace ds {
// -------------------------------------------------------
// ParticleAffector virtual base class 
// -------------------------------------------------------
class ParticleAffector {

public:
	ParticleAffector(const char* name) {
		m_HashName = string::murmur_hash(name);
	}
	virtual ~ParticleAffector() {}
	virtual void update(Particle* particle,float elapsed) = 0;	
	const IdString getHashName() const {
		return m_HashName;
	}
private:
	IdString m_HashName;
};

// -------------------------------------------------------
// Wiggle Affector
// -------------------------------------------------------
struct WiggleAffectorSettings : public DynamicSettings {

	float radius;
	float amplitude;
	float random;

	void load(NewSettingsReader& reader) {
		reader.get<float>("radius",&radius);
		reader.get<float>("amplitude",&amplitude);
		reader.get<float>("random",&random);
	}

};

class WiggleAffector : public ParticleAffector {

public:
	WiggleAffector(const char* name) : ParticleAffector(name) {
		m_Settings = new WiggleAffectorSettings;
		m_Settings->radius = 20.0f;
		m_Settings->amplitude = 2.0f;
	}
	WiggleAffector(const char* name,WiggleAffectorSettings* settings) : ParticleAffector(name) , m_Settings(settings) {}
	void update(Particle* particle,float elapsed);
private:
	WiggleAffectorSettings* m_Settings;

};

}
