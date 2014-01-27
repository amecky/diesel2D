#pragma once
#include "ParticleSystem.h"
#include "EmitterMesh.h"
#include "..\lib\container\List.h"
#include "..\io\Serializer.h"

namespace ds {


struct RotationPath : public Gizmo {

	FloatArray data;

	RotationPath() : Gizmo("rotation") {
		add("rotation",&data);
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

// -------------------------------------------------------
// NewParticleSystem
// -------------------------------------------------------
class NewParticleSystem : public Serializer {

public:
	NewParticleSystem(Renderer* renderer,uint32 maxParticles,int textureID,int blendState);
	~NewParticleSystem(void);
	void update(float elapsed);
	const uint32 numAlive() const {
		return m_Count;
	}
	void draw();
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
	void setPosition(const Vec2& pos) {
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
	void start(const Vec2& startPos,int forcedCount = -1);
	//void stop();
	ColorPath* getColorPath() {
		return m_ColorPath;
	}
	Vec2Path* createSizePath() {
		if ( m_SizePath == 0 ) {
			m_SizePath = new Vec2Path;
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
	//! Loads data from a json file
	/*
		\param filename the name of the json file include directories
	*/
	void load(const char* fileName);	
	//! Reloads data from a json file
	void reload(const char* fileName);
private:
	Vec2 m_Position;
	List<Particle> m_Particles;
	uint32 m_MaxParticles;
	uint32 m_Count;
	int m_TextureID;
	Camera2D* m_Camera;
	ParticleData* m_ParticleData;
	ParticleEmitterData* m_EmitterData;
	SpriteBatch* m_SpriteBatch;
	EmitterMesh* m_Emitter;
	ColorPath* m_ColorPath;
	Vec2Path* m_SizePath;
	FloatPath* m_RadialVelocityPath;
	
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