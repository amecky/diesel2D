#include "ParticleManagerConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

ParticleManagerConverter::ParticleManagerConverter(void)  : Converter("ParticleManagerConverter") {
}


ParticleManagerConverter::~ParticleManagerConverter(void) {
}

void ParticleManagerConverter::convert(JSONReader& reader,BinaryWriter& writer) {
	std::vector<Category*> categories = reader.getCategories();
	for ( size_t i = 0; i < categories.size(); ++i ) {
		Category* c = categories[i];			
		writer.startChunk(CHNK_PARTICLESYSTEM,1);
		writer.write(c->getInt("id",0));
		writer.write(c->getProperty("file"));
		writer.write(c->getInt("max",512));
		writer.closeChunk();			
	}	
}

}
