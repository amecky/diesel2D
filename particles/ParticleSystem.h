#pragma once
#include "ParticleSystem.h"
#include "EmitterMesh.h"
#include "..\lib\container\List.h"
#include "..\io\Serializer.h"
#include <IPath.h>
#include "..\base\GameObject.h"
#include "..\data\Gizmo.h"
#include "Particle.h"
#include "..\renderer\Camera2D.h"
#include "DynamicSettings.h"

namespace ds {


struct RotationPath {

	IPath<20,float> data;

	RotationPath() {}

	void load(BinaryLoader* loader) {
		int total = 0;
		loader->read(&total);
		float timestep = 0.0f;
		float value = 0.0f;
		for ( int i = 0; i < total; ++i ) {
			loader->read(&timestep);
			loader->read(&value);
			data.add(timestep,value);
		}
	}
};

struct SizePath : public Gizmo {

	IPath<20,Vector2f> data;

	SizePath() : Gizmo("size") {
		add("size",&data);
	}
};



struct WiggleSettings : public Gizmo {

	float radius;
	float amplitude;

	WiggleSettings() : Gizmo("wiggle") , radius(10.0f) , amplitude(5.0f) {
		add("radius",&radius);
		add("amplitude",&amplitude);
	}
};

struct TrailSettings : public Gizmo {

	float distance;
	int count;

	TrailSettings() : Gizmo("trail") , distance(10.0f) , count(1) {
		add("distance",&distance);
		add("count",&count);
	}
};

// -------------------------------------------------------
// NewParticleSystem
// -------------------------------------------------------
class ParticleSystem : public Serializer , public GameObject {

public:
	ParticleSystem() : GameObject() {
		m_ParticleData = new ParticleData;
		m_EmitterData = new ParticleEmitterData;		
	}
	~ParticleSystem(void);
	void init();
	void setMaxParticles(int maxParticles) {
		m_MaxParticles = maxParticles;
	}
	void setBlendState(int blendState) {
		
	}
	void update(float elapsed);
	const uint32 numAlive() const {
		return m_Count;
	}
	void render();
	void setEmitter(EmitterMesh* emitter) {
		m_Emitter = emitter;
	}
	//void emitParticles();
	void setParticleData(ParticleData* particleData);
	ParticleData* getParticleData() const {
		return m_ParticleData;
	}
	template<class PE>
	PE* getEmitter() {
		return static_cast<PE*>(m_Emitter);
	}
	void setEmitterData(ParticleEmitterData* emitterData);
	void setPosition(const Vector2f& pos) {
		m_Position = pos;
	}
	const uint32 numParticles() const {
		return m_Count;
	}
	const bool isActive() const {
		return numAlive() > 0;
	}
	//void resetEmitterCounter();
	void setCamera(Camera2D* camera) {
		m_Camera = camera;
	}
	void start(const Vector2f& startPos,int forcedCount = -1);
	//void stop();
	ColorPath* getColorPath() {
		return m_ColorPath;
	}
	IPath<20,Vector2f>* createSizePath() {
		if ( m_SizePath == 0 ) {
			m_SizePath = new IPath<20,Vector2f>;
		}
		else {
			m_SizePath->reset();
		}
		return m_SizePath;
	}
	FloatPath* createRadialVelocityPath() {
		if ( m_RadialVelocityPath == 0 ) {
			m_RadialVelocityPath = new FloatPath;
		}
		else {
			m_RadialVelocityPath->reset();
		}
		return m_RadialVelocityPath;
	}	
	const uint32 getMaxParticles() const {
		return m_MaxParticles;
	}
	void setTextureID(int textureID) {
		m_TextureID = textureID;
	}
	//void load(const char* fileName);	
	//void reload(const char* fileName);
	void load(BinaryLoader* loader);	
private:
	void clear();
	void buildTrailParticle(Particle* baseParticle);
	Vector2f m_Position;
	List<Particle> m_Particles;
	uint32 m_MaxParticles;
	uint32 m_Count;
	int m_TextureID;
	Camera2D* m_Camera;
	ParticleData* m_ParticleData;
	ParticleEmitterData* m_EmitterData;
	EmitterMesh* m_Emitter;
	ColorPath* m_ColorPath;
	//Vector2fPath* m_SizePath;
	IPath<20,Vector2f>* m_SizePath;
	FloatPath* m_RadialVelocityPath;
	bool m_UseTrail;
	TrailSettings m_TrailSettings;
	RotationPath m_RotationPath;
	bool m_Rotating;

	WiggleSettings m_WiggleSettings;
	bool m_UseWiggle;
	//FloatPath* m_RotationPath;
	// emitter
	float m_EmitFrequency;
	float m_EmitDelay;
	float m_EmitTimer;
};

}