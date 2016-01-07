#pragma once
#include "..\lib\collection_types.h"
#include "ParticleModifier.h"
#include "ParticleEmitter.h"

namespace ds {

	class NewParticleSystem;

	class ParticleSystemFactory {

	public:
		ParticleSystemFactory();
		~ParticleSystemFactory();

		bool addModifier(NewParticleSystem* system, ParticleModifierType type);
		bool addModifier(NewParticleSystem* system, const char* modifierName) const;
		ParticleModifierData* addModifier(NewParticleSystem* system, int chunkID);

		bool addGenerator(NewParticleSystem* system, const char* generatorName);
		ParticleGeneratorData* addGenerator(NewParticleSystem* system, int chunkID);

		const Array<std::string>& getModifierNames() const {
			return _known_modifier_names;
		}
		const Array<std::string>& getGeneratorNames() const {
			return _known_generator_names;
		}
	private:
		ParticleModifierData* createData(ParticleModifierType type) const;
		ParticleGeneratorData* createData(ParticleGeneratorType type);
		ParticleSystemFactory(const ParticleSystemFactory& other) {}
		Array<std::string> _known_modifier_names;
		Array<std::string> _known_generator_names;
		
		ParticleModifier** _known_modifiers;
		ParticleGenerator** _known_generators;
		int _count_modifiers;
		int _count_generators;
	};

}

