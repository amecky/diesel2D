#pragma once
#include "ParticleSystem.h"
#include "..\lib\container\List.h"
#include "..\compiler\DataFile.h"
#include <IPath.h>
#include "Particle.h"
#include "..\renderer\Camera2D.h"
#include "ParticleModifier.h"
#include "ParticleEmitter.h"
#include "..\utils\Profiler.h"
#include "ParticleSystemFactory.h"
#include "..\lib\collection_types.h"

namespace ds {

// -------------------------------------------------------
// New particle system
// -------------------------------------------------------
const int MAX_PARTICLES = 4096;

struct ParticleSystemData {

	uint32 id;
	uint32 maxParticles;
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
	NewParticleSystem(int id, const char* name, ParticleSystemFactory* factory);
	~NewParticleSystem();
	void clear();
	void update(float elapsed);
	void render();
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
	bool loadData(JSONReader& reader);
	const char* getFileName() const {
		return _json_name;
	}
	ParticleEmitterData& getEmitterData() {
		return _emitter_data;
	}
private:
	void tickEmitters(float dt);
	void initEmitterData();
	void emittParticles(ParticleEmitterInstance& instance, float dt, uint32* start, uint32* end);
	void emittParticles(const ParticleEmitterInstance& instance, int count, uint32* start, uint32* end, float dt);
	void prepareVertices();
	void buildVertices();

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

}