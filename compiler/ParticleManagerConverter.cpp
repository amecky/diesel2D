#include "ParticleManagerConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

ParticleManagerConverter::ParticleManagerConverter(void)  : Converter("ParticleManagerConverter") {
}


ParticleManagerConverter::~ParticleManagerConverter(void) {
}

void ParticleManagerConverter::convert(const char* fileName) {
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
			writer.startChunk(CHNK_PARTICLESYSTEM,1);
			writer.write(c->getInt("id",0));
			writer.write(c->getProperty("file"));
			writer.write(c->getInt("max",512));
			writer.closeChunk();			
		}
		writer.close();
	}
	else {
		LOGEC("ParticleManagerConverter") << "Cannot parse " << fileName;
	}
}

}
