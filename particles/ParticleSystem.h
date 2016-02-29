#pragma once
#include "ParticleSystem.h"
//#include "..\lib\container\List.h"
#include "..\io\DataFile.h"
//#include <IPath.h>
#include "Particle.h"
//#include "..\renderer\Camera2D.h"
#include "ParticleManager.h"
//#include "ParticleEmitter.h"
#include "..\utils\Profiler.h"
#include "ParticleSystemFactory.h"
#include "..\lib\collection_types.h"
#include "ParticleModule.h"

namespace ds {

	struct ParticleEvent {
		enum ParticleEventType {
			SYSTEM_STARTED,
			SYSTEM_KILLED,
			PARTICLE_EMITTED,
			PARTICLE_KILLED,
			EOL
		};
		ID instance;
		v2 pos;
		ParticleEventType type;
	};


// -------------------------------------------------------
// New particle system
// -------------------------------------------------------
	/*
struct ParticleSystemData {

	uint32 id;
	uint32 maxParticles;
	uint32 textureID;
	Texture texture;
};
*/
// -------------------------------------------------------
// Module instance
// -------------------------------------------------------
struct ModuleInstance {

	ParticleModule* module;
	ParticleModuleData* data;

	ModuleInstance() : module(0), data(0) {}
};

// -------------------------------------------------------
// Particle system
// -------------------------------------------------------
class ParticleSystem : public DataFile {

public:
	ParticleSystem(int id, const char* name, ParticleSystemFactory* factory);
	~ParticleSystem();
	void clear();
	void update(float elapsed, Array<ParticleEvent>& events);
	ID start(const v2& startPosition);
	void stop(ID id) {}
	void addModule(ParticleModule* module, ParticleModuleData* data) {
		if (_count_modules < 32) {
			ModuleInstance& instance = _module_instances[_count_modules++];
			instance.module = module;
			instance.data = data;
		}
	}
	const ModuleInstance& getModuleInstance(int id) const {
		return _module_instances[id];
	}
	void getModuleNames(Array<const char*>& names);
	ParticleModuleData* getData(const char* modifierName);
	ParticleModule* getModule(ParticleModuleType type);
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
		return _texture;
	}
	int getID() const {
		return _id;
	}
	void removeModuleByName(const char* name);

	bool saveData(JSONWriter& writer);
	const char* getFileName() const {
		return _json_name;
	}
	virtual bool loadData(const JSONReader& loader);
	bool isAlive() const {
		return _spawnerInstances.numObjects > 0 || m_Array.countAlive > 0;
	}
	void activateEvents() {
		_sendEvents = true;
	}
private:
	void updateSpawners(float dt);
	void initSpawner();
	void emittParticles(ParticleSpawnerInstance& instance, float dt, uint32* start, uint32* end);
	void emittParticles(const ParticleSpawnerInstance& instance, int count, uint32* start, uint32* end, float dt);
	void prepareVertices();

	//ParticleSystemData _system_data;
	ParticleSpawner _spawner;
	Texture _texture;
	ParticleArray m_Array;
	char m_DebugName[32];
	char _json_name[64];
	int _id;
	ModuleInstance _module_instances[32];
	int _count_modules;
	ParticleSystemFactory* _factory;
	SpawnerInstances _spawnerInstances;
	bool _sendEvents;

};
/*
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
const int MAX_PARTICLE_SYSTEM = 2048;

class NewParticleSystem : public DataFile {

public:
	NewParticleSystem(int id, const char* name, ParticleSystemFactory* factory);
	~NewParticleSystem();
	void clear();
	void update(float elapsed);
	ParticleSystemData& getParticleData() {
		return _system_data;
	}
	ID start(const Vector3f& startPosition);
	void stop(ID id) {}
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
	void getModifierNames(Array<const char*>& names);
	void getGeneratorNames(Array<const char*>& names);
	ParticleModifierData* getData(const char* modifierName);
	ParticleGeneratorData* getGeneratorData(const char* generatorName);
	ParticleModifier* getModifier(ParticleModifierType type);
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
		return _system_data.texture;
	}
	int getID() const {
		return _id;
	}
	void removeModifierByName(const char* name);

	bool saveData(JSONWriter& writer);
	const char* getFileName() const {
		return _json_name;
	}
	ParticleEmitterData& getEmitterData() {
		return _emitter_data;
	}
	virtual bool loadData(const JSONReader& loader);
private:
	void tickEmitters(float dt);
	void initEmitterData();
	void emittParticles(ParticleEmitterInstance& instance, float dt, uint32* start, uint32* end);
	void emittParticles(const ParticleEmitterInstance& instance, int count, uint32* start, uint32* end, float dt);
	void prepareVertices();

	ParticleSystemData _system_data;
	ParticleEmitterData _emitter_data;
	ParticleArray m_Array;
	char m_DebugName[32];
	char _json_name[64];
	int _id;
	ModifierInstance _modifier_instances[32];
	int _count_modifiers;
	GeneratorInstance _generator_instances[32];
	int _count_generators;
	ParticleSystemFactory* _factory;
	EmitterInstances _emitter_instances;
	
};
*/
}