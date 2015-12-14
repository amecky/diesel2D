#pragma once
#include <vector>
#include "ParticleModifier.h"
#include "ParticleEmitter.h"

namespace ds {

	class NewParticleSystem;

	class ParticleSystemFactory {

	public:
		ParticleSystemFactory();
		~ParticleSystemFactory();

		bool addModifier(NewParticleSystem* system, ParticleModifierType type);
		bool addModifier(NewParticleSystem* system, const char* modifierName);
		ParticleModifierData* addModifier(NewParticleSystem* system, int chunkID);

		bool addGenerator(NewParticleSystem* system, const char* generatorName);
		ParticleGeneratorData* addGenerator(NewParticleSystem* system, int chunkID);

		const std::vector<std::string>& getModifierNames() const {
			return _known_modifier_names;
		}
		const std::vector<std::string>& getGeneratorNames() const {
			return _known_generator_names;
		}
	private:
		ParticleModifierData* createData(ParticleModifierType type);
		ParticleGeneratorData* createData(ParticleGeneratorType type);
		ParticleSystemFactory(const ParticleSystemFactory& other) {}
		std::vector<std::string> _known_modifier_names;
		std::vector<std::string> _known_generator_names;
		
		ParticleModifier** _known_modifiers;
		ParticleGenerator** _known_generators;
		int _count_modifiers;
		int _count_generators;
	};

}

