#include "SpritesDescriptionConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

SpritesDescriptionConverter::SpritesDescriptionConverter() : Converter("SpritesDescriptionConverter") {
}


SpritesDescriptionConverter::~SpritesDescriptionConverter(void) {
}

void SpritesDescriptionConverter::convert(const char* fileName) {
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
			LOG << "Converting sprite: " << c->getName();
			writer.startChunk(CHNK_SPRITE_DESCRIPTION,1);
			writer.write(c->getInt("id",0));
			writer.write(c->getInt("texture_id",0));
			writer.write(c->getVector2f("position",Vector2f(100,100)));
			Rect r;
			c->getRect("texture_rect",&r);
			writer.write(r);
			writer.write(c->getFloat("rotation",0.0f));
			writer.write(c->getVector2f("velocity",Vector2f(0.0f,0.0f)));
			writer.write(c->getInt("index",0));
			writer.write(c->getInt("user_value",0));
			writer.write(c->getColor("color",Color(255,255,255,255)));
			writer.write(c->getVector2f("scale",Vector2f(1.0f,1.0f)));
			writer.write(c->getFloat("radius",5.0f));
			writer.closeChunk();	
		}
		writer.close();
	}
	else {
		LOGEC("SpritesDescriptionConverter") << "Cannot parse " << fileName;
	}
}

}
