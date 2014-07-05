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
#include "ParticleModifier.h"
#include "ParticleEmitter.h"
#include "..\utils\Profiler.h"

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

// -------------------------------------------------------
// New particle system
// -------------------------------------------------------
struct NewParticleSystemData {

	uint32 id;
	uint32 maxParticles;
	Rect textureRect;
	uint32 textureID;
};

class NewParticleSystem : public GameObject , public Serializer {

typedef std::vector<ParticleModifier*> Modifiers;

public:
	NewParticleSystem() : GameObject() {
		m_Array.initialize(4096);
	}
	virtual ~NewParticleSystem() {
		clear();
	}

	void init() {}

	void clear() {
		Modifiers::iterator it = m_Modifiers.begin();
		while ( it != m_Modifiers.end()) {
			delete (*it);
			it = m_Modifiers.erase(it);
		}
		m_Emitter.clear();
	}

	void init(const NewParticleSystemData& data) {
		m_Data = data;
	}

	void init(const Rect& r,uint32 textureID) {
		m_Data.textureID = textureID;
		m_Data.textureRect = r;
	}

	void update(float elapsed) {
		//m_Emitter.generate(&m_Array,m_Position,elapsed);
		for ( size_t i = 0;i < m_Modifiers.size(); ++i ) {
			m_Modifiers[i]->update(&m_Array,elapsed);
		}
	}
	void render() {
		PR_START("NewParticleSystem::render")
		if ( m_Array.countAlive > 0 ) {		
			for ( uint32 i = 0; i < m_Array.countAlive; ++i ) {
				float sizeX = m_Array.scale[i].x;
				float sizeY = m_Array.scale[i].y;
				m_Renderer->draw(m_Array.position[i],m_Data.textureID,m_Data.textureRect,m_Array.rotation[i],sizeX,sizeY,m_Array.color[i]);
			}
		}
		PR_END("NewParticleSystem::render")
	}
	ParticleEmitter& getEmitter() {
		return m_Emitter;
	}
	NewParticleSystemData& getParticleData() {
		return m_Data;
	}
	void addGenerator(ParticleGenerator* generator) {
		m_Emitter.add(generator);
}
	void start(const Vector2f& startPosition) {
		m_Position = startPosition;
		m_Emitter.generate(&m_Array,m_Position,0.0f);
	}
	void addModifier(ParticleModifier* modifier) {
		m_Modifiers.push_back(modifier);
	}
	void setPosition(const Vector2f& position) {
		m_Position = position;
	}
	void load(BinaryLoader* loader);
private:
	Vector2f m_Position;
	NewParticleSystemData m_Data;
	ParticleSettings m_Settings;
	ParticleArray m_Array;
	Modifiers m_Modifiers;
	ParticleEmitter m_Emitter;
};

}