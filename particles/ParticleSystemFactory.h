#pragma once
#include <vector>
#include "ParticleModifier.h"

namespace ds {

	class NewParticleSystem;

	class ParticleSystemFactory {

	public:
		ParticleSystemFactory();
		~ParticleSystemFactory();

		bool addModifier(NewParticleSystem* system, ParticleModifierType type);
		bool addModifier(NewParticleSystem* system, const char* modifierName);

		const std::vector<std::string>& getModifierNames() const {
			return _known_modifier_names;
		}
	private:
		ParticleSystemFactory(const ParticleSystemFactory& other) {}
		std::vector<std::string> _known_modifier_names;
		ParticleModifierData* createData(ParticleModifierType type);
		ParticleModifier** _known_modifiers;
		int _count_modifiers;
	};

}

