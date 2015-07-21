#include "NewParticleSystemConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\particles\ParticleModifier.h"
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
				else if (c->getName() == "alpha_path_modifier") {
					writer.startChunk(109, 1);
					AlphaPathModifier lm;
					lm.convert(c, writer);
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
			writer.write(emitter->getFloat("duration", 0.0f));
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
				else if (c->getName() == "color_generator") {
					writer.startChunk(205, 1);
					ColorGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				else if (c->getName() == "hsv_color_generator") {
					writer.startChunk(206, 1);
					HSVColorGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				else if (c->getName() == "point_generator") {
					writer.startChunk(207, 1);
					PointGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				if (c->getName() == "circle_position_generator") {
					writer.startChunk(208, 1);
					CircleGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				else if (c->getName() == "line_position_generator") {
					writer.startChunk(209, 1);
					LineGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				else if (c->getName() == "velocity_generator") {
					writer.startChunk(210, 1);
					VelocityGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				else if (c->getName() == "sphere_generator") {
					writer.startChunk(211, 1);
					SphereGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
				else if (c->getName() == "random_sphere_generator") {
					writer.startChunk(212, 1);
					RandomSphereGenerator rg;
					rg.convert(c, writer);
					writer.closeChunk();
				}
			}
		}		
	}
}

}