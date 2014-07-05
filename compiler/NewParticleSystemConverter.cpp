#include "NewParticleSystemConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\particles\ParticleModifier.h"
#include "..\particles\EmitterMesh.h"
#include "..\particles\ParticleEmitter.h"

namespace ds {

void NewParticleSystemConverter::convert(JSONReader& reader,BinaryWriter& writer) {	
	Category* root = reader.getCategory("particlesystem");
	if ( root != 0 ) {
		writer.startChunk(400,1);
		writer.write(root->getUInt32("id",0));
		writer.write(root->getUInt32("texture_id",0));
		writer.write(root->getRect("texture_rect"));
		writer.closeChunk();
		Category* modifiers = root->getChild("modifiers");
		if (  modifiers != 0 ) {
			std::vector<Category*> categories = modifiers->getChildren();
			for ( size_t i = 0; i < categories.size(); ++i ) {
				Category* c = categories[i];			
				if ( c->getName() == "lifecycle_modifier" ) {		
					writer.startChunk(100,1);
					writer.closeChunk();
				}
				else if ( c->getName() == "position_modifier" ) {		
					writer.startChunk(101,1);
					writer.closeChunk();
				}
				else if ( c->getName() == "linear_color_modifier" ) {		
					writer.startChunk(102,1);
					LinearColorModifier lm;
					lm.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "linear_size_modifier" ) {		
					writer.startChunk(103,1);
					LinearSizeModifier lm;
					lm.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "perpendicular_position_modifier" ) {		
					writer.startChunk(104,1);
					PerpendicularMoveModifier lm;
					lm.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "color_path_modifier" ) {		
					writer.startChunk(105,1);
					ColorPathModifier lm;
					lm.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "damping_velocity_modifier" ) {		
					writer.startChunk(106,1);
					DampingVelocityModifier lm;
					lm.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "size_path_modifier" ) {		
					writer.startChunk(107,1);
					SizePathModifier lm;
					lm.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "velocity_rotation_modifier" ) {		
					writer.startChunk(108,1);
					writer.closeChunk();
				}
			}
		}
		Category* emitter = root->getChild("emitter");
		if (  emitter != 0 ) {
			std::vector<Category*> categories = emitter->getChildren();
			writer.startChunk(300,1);
			writer.write(emitter->getUInt32("count",10));
			writer.write(emitter->getUInt32("ejection_period",0));
			writer.write(emitter->getUInt32("ejection_variance",0));
			writer.write(emitter->getUInt32("ejection_counter",0));
			writer.closeChunk();
			for ( size_t i = 0; i < categories.size(); ++i ) {
				Category* c = categories[i];			
				if ( c->getName() == "ring_position_generator" ) {		
					writer.startChunk(200,1);
					RingGenerator rg;
					rg.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "random_generator" ) {		
					writer.startChunk(201,1);
					ParticleRandomGenerator rg;
					rg.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "lifecycle_generator" ) {		
					writer.startChunk(202,1);
					LifetimeGenerator rg;
					rg.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "radial_velocity_generator" ) {		
					writer.startChunk(203,1);
					RadialVelocityGenerator rg;
					rg.convert(c,writer);
					writer.closeChunk();
				}
				else if ( c->getName() == "size_generator" ) {		
					writer.startChunk(204,1);
					SizeGenerator rg;
					rg.convert(c,writer);
					writer.closeChunk();
				}
			}
		}		
	}
}

}