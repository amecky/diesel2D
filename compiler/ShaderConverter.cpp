#include "ShaderConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

ShaderConverter::ShaderConverter() : Converter("ShaderConverter") {
}


ShaderConverter::~ShaderConverter(void) {
}

void ShaderConverter::convert(JSONReader& reader,BinaryWriter& writer) {	
	Category* c = reader.getCategory("shader");
	writer.startChunk(CHNK_SPRITE,1);
	writer.write(c->getProperty("name"));
	writer.write(c->getProperty("tech"));
	writer.write(c->getProperty("code"));	
	writer.closeChunk();			
}

}
