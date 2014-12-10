#include "HUDConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

HUDConverter::HUDConverter(void) : Converter("HUDConverter") {
}


HUDConverter::~HUDConverter(void) {
}

void HUDConverter::convert(JSONReader& reader,BinaryWriter& writer) {	
	std::vector<Category*> categories = reader.getCategories();
	for ( size_t i = 0; i < categories.size(); ++i ) {
		Category* c = categories[i];			
		if (c->getName() == "number_definitions") {
			writer.startChunk(5, 1);
			std::string str = "0";
			for (int i = 0; i < 10; ++i) {
				char ch = '0' + i;
				str[0] = ch;
				assert(c->hasProperty(str));
				writer.write(c->getRect(str));
			}
			writer.closeChunk();
		}
		else {
			int id = c->getInt(0, "id");
			Vector2f pos = c->getVector2f("position");
			Color clr = Color::WHITE;
			c->getColor("color", &clr);
			float scale = 1.0f;
			c->getFloat("scale", &scale);
			if (c->getName() == "counter") {
				int length = 6;
				c->getInt("length", &length);
				writer.startChunk(1, 1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(length);
				writer.closeChunk();
			}
			else if (c->getName() == "timer") {
				writer.startChunk(2, 1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.closeChunk();
			}
			else if (c->getName() == "text") {
				writer.startChunk(3, 1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(c->getProperty("text"));
				writer.closeChunk();
			}
			else if (c->getName() == "image") {
				Rect rect;
				c->getRect("rect", &rect);
				writer.startChunk(4, 1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(rect);
				writer.closeChunk();
			}
			else if (c->getName() == "number") {
				int length = -1;
				c->getInt("length", &length);
				writer.startChunk(6, 1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(length);
				writer.closeChunk();
			}
		}
	}				
}

}