#include "ParticleSystem.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\utils\Profiler.h"

namespace ds {

void NewParticleSystem::load(BinaryLoader* loader) {
	clear();
	while ( loader->openChunk() == 0 ) {	
		// modifiers
		if ( loader->getChunkID() == 100 ) {	
			ParticleTimeModifier* modifier = new ParticleTimeModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 101 ) {	
			ParticlePositionModifier* modifier = new ParticlePositionModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 102 ) {	
			LinearColorModifier* modifier = new LinearColorModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 103 ) {	
			LinearSizeModifier* modifier = new LinearSizeModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 104 ) {	
			PerpendicularMoveModifier* modifier = new PerpendicularMoveModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 105 ) {	
			ColorPathModifier* modifier = new ColorPathModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 106 ) {	
			DampingVelocityModifier* modifier = new DampingVelocityModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 107 ) {	
			SizePathModifier* modifier = new SizePathModifier;
			modifier->load(loader);
			addModifier(modifier);
		}
		else if ( loader->getChunkID() == 108 ) {	
			VelocityRotationModifier* modifier = new VelocityRotationModifier;
			addModifier(modifier);
		}
		// generators
		else if ( loader->getChunkID() == 200 ) {	
			RingGenerator* generator = new RingGenerator;
			generator->load(loader);
			addGenerator(generator);
		}
		else if ( loader->getChunkID() == 201 ) {	
			ParticleRandomGenerator* generator = new ParticleRandomGenerator;
			generator->load(loader);
			addGenerator(generator);
		}
		else if ( loader->getChunkID() == 202 ) {
			LifetimeGenerator* generator = new LifetimeGenerator;
			generator->load(loader);
			addGenerator(generator);
		}
		else if ( loader->getChunkID() == 203 ) {
			RadialVelocityGenerator* generator = new RadialVelocityGenerator;
			generator->load(loader);
			addGenerator(generator);
		}
		else if ( loader->getChunkID() == 204 ) {
			SizeGenerator* generator = new SizeGenerator;
			generator->load(loader);
			addGenerator(generator);
		}
		else if ( loader->getChunkID() == 300 ) {
			ParticleEmitterData& data = m_Emitter.getEmitterData();
			loader->read(&data.count);
		}
		else if ( loader->getChunkID() == 400 ) {
			loader->read(&m_Data.id);
			loader->read(&m_Data.textureID);
			loader->read(&m_Data.textureRect);
		}
		loader->closeChunk();
	}		
}

}
