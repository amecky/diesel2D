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

	ModifierInstance() : modifier(0), data(0) {}
};

// -------------------------------------------------------
// Generator instance
// -------------------------------------------------------
struct GeneratorInstance {
	ParticleGenerator* generator;
	ParticleGeneratorData* data;

	GeneratorInstance() : generator(0), data(0) {}
};

// -------------------------------------------------------
// Particle system
// -------------------------------------------------------
class NewParticleSystem : public DataFile {

public:
	NewParticleSystem(int id,const char* name, ParticleSystemFactory* factory) {
		strcpy_s(m_DebugName, 32, name);
		sprintf_s(_json_name, 64, "particles\\%s.json", name);
		_id = id;
		m_Array.initialize(MAX_PARTICLES);
		_count_modifiers = 0;
		_count_generators = 0;
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

	//ParticleEmitter& getEmitter() {
		//return m_Emitter;
	//}
	NewParticleSystemData& getParticleData() {
		return m_Data;
	}
	void addGenerator(ParticleGenerator* generator) {
		//m_Emitter.add(generator);
	}

	ID start(const Vector3f& startPosition);

	void stop() {
		//m_Emitter.stop();
	}

	void addModifier(ParticleModifier* modifier,ParticleModifierData* data) {
		ModifierInstance& instance = _modifier_instances[_count_modifiers++];
		instance.modifier = modifier;
		instance.data = data;
	}

	void addGenerator(ParticleGenerator* generator, ParticleGeneratorData* data) {
		GeneratorInstance& instance = _generator_instances[_count_generators++];
		instance.generator = generator;
		instance.data = data;
	}

	const ModifierInstance& getModifierInstance(int id) const {
		return _modifier_instances[id];
	}

	const GeneratorInstance& getGeneratorInstance(int id) const {
		return _generator_instances[id];
	}

	void getModifierNames(std::vector<std::string>& names);

	void getGeneratorNames(std::vector<std::string>& names);

	ParticleModifierData* getData(const char* modifierName);

	ParticleGeneratorData* getGeneratorData(const char* generatorName);

	ParticleModifier* getModifier(ParticleModifierType type);

	void setPosition(const Vector3f& position) {
		//_generatorData.position = position;
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

	ParticleEmitterData& getEmitterData() {
		return _emitter_data;
	}
private:
	void tickEmitters(float dt);
	void initEmitterData();
	void emittParticles(ParticleEmitterInstance& instance, float dt, uint32* start, uint32* end);
	void emittParticles(const ParticleEmitterInstance& instance, int count, uint32* start, uint32* end, float dt);

	ParticleGenerator* createGenerator(int id);
	ParticleModifier* createModifier(int id);
	void prepareVertices();
	void buildVertices();
	NewParticleSystemData m_Data;
	ParticleSettings m_Settings;
	ParticleArray m_Array;
	char m_DebugName[32];
	char _json_name[64];
	int _id;
	
	int dataIndices[32];
	int numModifiers;

	ModifierInstance _modifier_instances[32];
	int _count_modifiers;
	GeneratorInstance _generator_instances[32];
	int _count_generators;
	ParticleSystemFactory* _factory;
	EmitterInstances _emitter_instances;
	ParticleEmitterData _emitter_data;
};

}