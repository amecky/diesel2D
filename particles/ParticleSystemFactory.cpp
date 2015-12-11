#include "ParticleSystemFactory.h"
#include "ParticleSystem.h"

namespace ds {

	ParticleSystemFactory::ParticleSystemFactory() {
		_count_modifiers = 0;
		_known_modifiers = new ParticleModifier*[32];
		_known_modifiers[_count_modifiers++] = new ParticleTimeModifier();
		_known_modifiers[_count_modifiers++] = new ParticlePositionModifier();
		for (int i = 0; i < _count_modifiers; ++i) {
			_known_modifier_names.push_back(_known_modifiers[i]->getName());
		}
	}


	ParticleSystemFactory::~ParticleSystemFactory()	{
		for (int i = 0; i < _count_modifiers; ++i) {
			delete _known_modifiers[i];
		}
		delete[] _known_modifiers;
	}

	bool ParticleSystemFactory::addModifier(NewParticleSystem* system, ParticleModifierType type) {
		for (int i = 0; i < _count_modifiers; ++i) {
			if (_known_modifiers[i]->getType() == type) {
				ParticleModifierData* data = createData(type);
				system->addModifier(_known_modifiers[i],data);
				return true;
			}
		}
		return false;
	}

	bool ParticleSystemFactory::addModifier(NewParticleSystem* system, const char* modifierName) {
		for (int i = 0; i < _count_modifiers; ++i) {
			if (strcmp(_known_modifiers[i]->getName(),modifierName) == 0) {
				ParticleModifierData* data = createData(_known_modifiers[i]->getType());
				system->addModifier(_known_modifiers[i],data);
				return true;
			}
		}
		return false;
	}

	ParticleModifierData* ParticleSystemFactory::createData(ParticleModifierType type) {
		switch (type) {
			case PMT_DAMPING_VELOCITY: return new DampingVelocityModifierData(); break;
		}
		return 0;
	}

	
}
