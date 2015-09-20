#pragma once
#include "ParticleSystem.h"
#include "..\lib\container\List.h"
#include "..\io\Serializer.h"
#include <IPath.h>
#include "..\data\Gizmo.h"
#include "Particle.h"
#include "..\renderer\Camera2D.h"
#include "DynamicSettings.h"
#include "ParticleModifier.h"
#include "ParticleEmitter.h"
#include "..\utils\Profiler.h"
#include "..\compiler\AssetCompiler.h"

namespace ds {
	
	struct ModifierData {

		char* buffer;
		int index;
		int num;
		int entries[64];

		ModifierData() : buffer(0) , index(0) , num(0) {}

		~ModifierData() {
			if (buffer != 0) {
				delete[] buffer;
			}
		}

		void allocate(int size) {
			buffer = new char[size];
		}

		template<class T>
		const bool get(int id,T* t,int size) const {
			int idx = entries[id];
			char* data = buffer + idx;
			memcpy(t, data, size);
			return true;
		}

		template<class T>
		int add(const T& t) {
			int current = index;
			entries[num++] = current;
			char* data = buffer + index;
			memcpy(data, &t, sizeof(t));
			index += sizeof(t);
			return current;
		}
	};

	typedef void(*PModifier)(const ModifierData&, ParticleArray*, float);
	typedef void(*PGenerator)(const ModifierData&, ParticleArray*, const Vector3f&, float, uint32, uint32);

	struct LiveOverTimeData {

		float ttl;

	};

	void lifeOverTime(const ModifierData& data, ParticleArray* array, float dt);

	void move(const ModifierData& data, ParticleArray* array, float dt);

// -------------------------------------------------------
// New particle system
// -------------------------------------------------------
const int MAX_PARTICLES = 4096;

struct NewParticleSystemData {

	uint32 id;
	uint32 maxParticles;
	Rect textureRect;
	uint32 textureID;
	Texture texture;
};

class NewParticleSystem : public Serializer {

typedef std::vector<ParticleModifier*> Modifiers;
//typedef std::vector<ParticleEmitterInstance> EmitterInstances;

public:
	NewParticleSystem() {
		m_Array.initialize(MAX_PARTICLES);
		
	}
	virtual ~NewParticleSystem() {
		clear();
	}

	void clear();

	void init(const NewParticleSystemData& data) {
		m_Data = data;
	}

	void init(const Rect& r,uint32 textureID) {
		m_Data.textureID = textureID;
		m_Data.textureRect = r;		
	}

	template<class T>
	void addPM(const PModifier& modifier,const T& t) {
		modifiers[numModifiers] = modifier;
		dataIndices[numModifiers] = m_ModifierData.add(t);
		++numModifiers;
	}

	void addPM(const PModifier& modifier) {
		modifiers[numModifiers] = modifier;
		dataIndices[numModifiers] = -1;
		++numModifiers;
	}

	void update(float elapsed);

	void render();

	ParticleEmitter& getEmitter() {
		return m_Emitter;
	}
	NewParticleSystemData& getParticleData() {
		return m_Data;
	}
	void addGenerator(ParticleGenerator* generator) {
		m_Emitter.add(generator);
	}

	void start(const Vector3f& startPosition);

	void start(const ParticleGeneratorData& data);

	void stop() {
		m_Emitter.stop();
	}
	void addModifier(ParticleModifier* modifier) {
		m_Modifiers.push_back(modifier);
	}
	void setPosition(const Vector3f& position) {
		_generatorData.position = position;
	}
	void load(BinaryLoader* loader);
	void setDebugName(const char* name);
	const int getCountAlive() const {
		return m_Array.countAlive;
	}
	const char* getDebugName() const {
		return m_DebugName;
	}
	const ParticleArray& getArray() const {
		return m_Array;
	}
	const Texture& getTexture() const {
		return m_Data.texture;
	}
private:
	ParticleGenerator* createGenerator(int id);
	ParticleModifier* createModifier(int id);
	void prepareVertices();
	void buildVertices();
	ParticleGeneratorData _generatorData;
	NewParticleSystemData m_Data;
	ParticleSettings m_Settings;
	ParticleArray m_Array;
	Modifiers m_Modifiers;
	ParticleEmitter m_Emitter;
	char m_DebugName[32];
	//EmitterInstances m_Instances;

	PModifier modifiers[32];
	int dataIndices[32];
	int numModifiers;
	ModifierData m_ModifierData;

	
};

}