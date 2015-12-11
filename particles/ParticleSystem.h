#pragma once
#include "ParticleSystem.h"
#include "..\lib\container\List.h"
#include "..\compiler\DataFile.h"
#include <IPath.h>
#include "..\data\Gizmo.h"
#include "Particle.h"
#include "..\renderer\Camera2D.h"
#include "DynamicSettings.h"
#include "ParticleModifier.h"
#include "ParticleEmitter.h"
#include "..\utils\Profiler.h"
#include "..\compiler\AssetCompiler.h"
#include "ParticleSystemFactory.h"

namespace ds {

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

// -------------------------------------------------------
// Modifier instance
// -------------------------------------------------------
struct ModifierInstance {

	ParticleModifier* modifier;
	ParticleModifierData* data;
};


// -------------------------------------------------------
// Generator instance
// -------------------------------------------------------
struct GeneratorInstance {

	int data_index;
	ParticleGenerator* generator;

};

typedef std::vector<ParticleModifier*> Modifiers;

class NewParticleSystem : public DataFile {

public:
	NewParticleSystem(int id,const char* name, ParticleSystemFactory* factory) {
		strcpy_s(m_DebugName, 32, name);
		sprintf_s(_json_name, 64, "particles\\%s.json", name);
		_id = id;
		m_Array.initialize(MAX_PARTICLES);
		_count_modifiers = 0;
		_factory = factory;
		
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

	void addModifier(ParticleModifier* modifier,ParticleModifierData* data) {
		ModifierInstance& instance = _modifier_instances[_count_modifiers++];
		instance.modifier = modifier;
		instance.data = data;
	}

	ParticleModifierData* getData(const char* modifierName);

	ParticleModifier* getModifier(ParticleModifierType type);

	void setPosition(const Vector3f& position) {
		_generatorData.position = position;
	}
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
	const Modifiers& getModifiers() const {
		return m_Modifiers;
	}
	int getID() const {
		return _id;
	}
	void removeModifierByName(const char* name);

	bool exportData(JSONWriter& writer);
	bool importData(JSONReader& reader);
	bool saveData(BinaryWriter& writer);
	bool loadData(BinaryLoader& loader);
	const char* getJSONFileName() const {
		return _json_name;
	}
	const char* getFileName() const {
		return m_DebugName;
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
	char _json_name[64];
	int _id;
	
	int dataIndices[32];
	int numModifiers;

	ModifierInstance _modifier_instances[32];
	int _count_modifiers;
	ParticleSystemFactory* _factory;
};

}