#include "ParticleSystemConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

void ParticleSystemConverter::convert(const char* fileName) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",fileName);
	char out[256];
	sprintf(out,"data\\%s.dsb",fileName);
	BinaryWriter writer;
	int signature[] = {0,8,15};
	writer.open(out,signature,3);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];			
			if ( c->getName() == "particle_data" ) {				
				Rect r;
				c->getRect("texture_rect",&r);	
				writer.startChunk(CHNK_PARTICLE_DATA,1);
				writer.write(r);
				writer.write(c->getVector2f("initial_size",Vector2f(1.0f,1.0f)));
				writer.write(c->getColor("start_color"));
				writer.write(c->getColor("end_color"));
				writer.write(c->getFloat("min_rotation_speed",0.0f));
				writer.write(c->getFloat("max_rotation_speed",0.0f));
				writer.write(c->getFloat("random",0.0f));
				writer.closeChunk();
			}			
			else if ( c->getName() == "emitter_data" ) {			
				writer.startChunk(CHNK_EMITTER_DATA,1);
				writer.write(c->getInt("count",32));
				writer.write(c->getFloat("velocity",10.0f));
				writer.write(c->getFloat("velocity_variance",0.0f));
				writer.write(c->getUInt32("ejection_period",0));
				writer.write(c->getUInt32("ejection_variance",0));
				writer.write(c->getUInt32("ejection_counter",0));
				writer.write(c->getFloat("ttl",1.0f));
				writer.write(c->getFloat("ttl_variance",1.0f));
				writer.write(c->getFloat("size_min_variance",0.0f));
				writer.write(c->getFloat("size_max_variance",0.0f));
				writer.closeChunk();
			}			
			else if ( c->getName() == "ring_emitter" ) {		
				writer.startChunk(CHNK_RING_EMITTER,1);
				writer.write(c->getInt("radius",10));
				writer.write(c->getFloat("angle_variance",0.0f));
				writer.write(c->getFloat("start_angle",0.0f));
				writer.write(c->getFloat("start_variance",0.0f));
				writer.write(c->getInt("radius_variance",0));				
				writer.write(c->getFloat("normal_variance",0.0f));				
				writer.closeChunk();
			}	
			else if ( c->getName() == "box_emitter" ) {		
				writer.startChunk(CHNK_BOX_EMITTER,1);
				writer.write(c->getInt("width",10));
				writer.write(c->getInt("height",10));
				writer.write(c->getInt("size",10));
				writer.write(c->getFloat("angle_variance",0.0f));
				writer.closeChunk();
			}	
			else if ( c->getName() == "size") {			
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				int total = propertyNames.size();
				writer.startChunk(CHNK_PARTICLE_SIZE,1);
				writer.write(total);
				for ( size_t i = 0; i < total; ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					writer.write(timeStep);
					writer.write(c->getVector2f(propertyNames[i]));
				}
				writer.closeChunk();
			}
			else if ( c->getName() == "color") {			
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				int total = propertyNames.size();
				writer.startChunk(CHNK_PARTICLE_COLOR,1);
				writer.write(total);
				for ( size_t i = 0; i < total; ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					writer.write(timeStep);
					writer.write(c->getColor(propertyNames[i]));
				}
				writer.closeChunk();
			}
			if ( c->getName() == "radial_velocity") {
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				int total = propertyNames.size();
				writer.startChunk(CHNK_PARTICLE_VELOCITY,1);
				writer.write(total);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					writer.write(timeStep);
					writer.write(c->getFloat(propertyNames[i],1.0f));					
				}
				writer.closeChunk();
			}	
			if ( c->getName() == "rotation") {	
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				int total = propertyNames.size();
				writer.startChunk(CHNK_PARTICLE_ROTATION,1);
				writer.write(total);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					writer.write(timeStep);
					writer.write(c->getFloat(propertyNames[i],1.0f));					
				}
				writer.closeChunk();			
			}	
		}		
		writer.close();
	}
}

}