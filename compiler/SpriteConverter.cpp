#include "SpriteConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

SpriteConverter::SpriteConverter() : Converter("SpriteConverter") {
}


SpriteConverter::~SpriteConverter(void) {
}

void SpriteConverter::convert(JSONReader& reader,BinaryWriter& writer) {	
	std::vector<Category*> categories  = reader.getCategories();
	for (size_t i = 0; i < categories.size(); ++i) {
		Category* c = categories[i];
		if (c->getName() == "sprite") {
			writer.startChunk(CHNK_SPRITE, 1);
			writer.write(c->getInt("id", -1));
			writer.write(c->getInt("texture_id", 0));
			writer.write(c->getVector2f("position", Vector2f(100, 100)));
			Rect r;
			c->getRect("rect", &r);
			writer.write(r);
			writer.write(c->getVector2f("scale", Vector2f(1.0f, 1.0f)));
			writer.write(c->getFloat("rotation", 0.0f));
			writer.write(c->getColor("color", Color(255, 255, 255, 255)));
			writer.closeChunk();
		}
	}
}

}
