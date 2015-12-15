#include "ParticleSystemFactory.h"
#include "ParticleSystem.h"

namespace ds {

	ParticleSystemFactory::ParticleSystemFactory() {
		_count_modifiers = 0;
		_known_modifiers = new ParticleModifier*[32];
		_known_generators = new ParticleGenerator*[32];
		_known_modifiers[_count_modifiers++] = new ParticleTimeModifier();
		_known_modifiers[_count_modifiers++] = new ParticlePositionModifier();
		_known_modifiers[_count_modifiers++] = new DampingVelocityModifier();
		_known_modifiers[_count_modifiers++] = new ColorPathModifier();
		_known_modifiers[_count_modifiers++] = new VelocityRotationModifier();
		_known_modifiers[_count_modifiers++] = new LinearSizeModifier();
		_known_modifiers[_count_modifiers++] = new LinearAlphaModifier();
		_known_modifiers[_count_modifiers++] = new AlphaPathModifier();
		_known_modifiers[_count_modifiers++] = new LinearColorModifier();
		_known_modifiers[_count_modifiers++] = new SizePathModifier();
		for (int i = 0; i < _count_modifiers; ++i) {
			_known_modifier_names.push_back(_known_modifiers[i]->getName());
		}
		_count_generators = 0;
		_known_generators[_count_generators++] = new RingGenerator();
		_known_generators[_count_generators++] = new RadialVelocityGenerator();
		_known_generators[_count_generators++] = new LifetimeGenerator();
		_known_generators[_count_generators++] = new SizeGenerator();
		_known_generators[_count_generators++] = new ColorGenerator();
		_known_generators[_count_generators++] = new HSVColorGenerator();
		for (int i = 0; i < _count_generators; ++i) {
			_known_generator_names.push_back(_known_generators[i]->getName());
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

	ParticleModifierData* ParticleSystemFactory::addModifier(NewParticleSystem* system, int chunkID) {
		for (int i = 0; i < _count_modifiers; ++i) {
			if (_known_modifiers[i]->getChunkID() == chunkID ) {
				ParticleModifierData* data = createData(_known_modifiers[i]->getType());
				system->addModifier(_known_modifiers[i], data);
				return data;
			}
		}
		return 0;
	}

	bool ParticleSystemFactory::addGenerator(NewParticleSystem* system, const char* generatorName) {
		for (int i = 0; i < _count_generators; ++i) {
			if (strcmp(_known_generators[i]->getName(), generatorName) == 0) {
				ParticleGeneratorData* data = createData(_known_generators[i]->getType());
				system->addGenerator(_known_generators[i], data);
				return true;
			}
		}
		return false;
	}

	ParticleGeneratorData* ParticleSystemFactory::addGenerator(NewParticleSystem* system, int chunkID) {
		for (int i = 0; i < _count_generators; ++i) {
			if (_known_generators[i]->getChunkID() == chunkID) {
				ParticleGeneratorData* data = createData(_known_generators[i]->getType());
				system->addGenerator(_known_generators[i], data);
				return data;
			}
		}
		return 0;
	}

	ParticleGeneratorData* ParticleSystemFactory::createData(ParticleGeneratorType type) {
		switch (type) {
			case PGT_RING: return new RingGeneratorData(); break;
			case PGT_RADIAL_VELOCITY: return new RadialVelocityGeneratorData(); break;
			case PGT_LIFETIME: return new LifetimeGeneratorData(); break;
			case PGT_SIZE: return new SizeGeneratorData(); break;
			case PGT_COLOR: return new ColorGeneratorData(); break;
			case PGT_HSV_COLOR: return new HSVColorGeneratorData(); break;
		}
		return 0;
	}

	ParticleModifierData* ParticleSystemFactory::createData(ParticleModifierType type) {
		switch (type) {
			case PMT_DAMPING_VELOCITY: return new DampingVelocityModifierData(); break;
			case PMT_LINEAR_SIZE: return new LinearSizeModifierData(); break;
			case PMT_COLOR_PATH: return new ColorPathModifierData(); break;
			case PMT_LINEAR_COLOR: return new LinearColorModifierData(); break;
			case PMT_LINEAR_ALPHA: return new LinearAlphaModifierData(); break;
		}
		return 0;
	}

	
}
