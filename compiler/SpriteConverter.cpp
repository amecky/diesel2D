#include "SpriteConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

SpriteConverter::SpriteConverter() : Converter("SpriteConverter") {
}


SpriteConverter::~SpriteConverter(void) {
}

void SpriteConverter::convert(const char* fileName) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",fileName);
	char out[256];
	sprintf(out,"data\\%s.dsb",fileName);
	BinaryWriter writer;
	int signature[] = {0,8,15};
	writer.open(out,signature,3);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		Category* c = reader.getCategory("sprite");
		writer.startChunk(CHNK_SPRITE,1);
		writer.write(c->getInt("texture_id",0));
		writer.write(c->getVector2f("position",Vector2f(100,100)));
		Rect r;
		c->getRect("rect",&r);
		writer.write(r);
		writer.write(c->getVector2f("scale",Vector2f(1.0f,1.0f)));
		writer.write(c->getColor("color",Color(255,255,255,255)));
		writer.write(c->getFloat("rotation",0.0f));
		writer.closeChunk();			
		writer.close();
	}
	else {
		LOGEC("SpriteConverter") << "Cannot parse " << fileName;
	}
}

}
