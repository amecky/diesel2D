#include "HUDConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

HUDConverter::HUDConverter(void) : Converter("HUDConverter") {
}


HUDConverter::~HUDConverter(void) {
}

void HUDConverter::convert(const char* fileName) {
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
			int id = c->getInt(0,"id");
			Vector2f pos = c->getVector2f("position");			
			Color clr = Color::WHITE;
			c->getColor("color",&clr);
			float scale = 1.0f;
			c->getFloat("scale",&scale);
			if ( c->getName() == "counter" ) {				
				int length = 6;
				c->getInt("length",&length);
				writer.startChunk(1,1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(length);
				writer.closeChunk();
			}
			else if ( c->getName() == "timer" ) {
				writer.startChunk(2,1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.closeChunk();
			}
			else if ( c->getName() == "text" ) {
				writer.startChunk(3,1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(c->getProperty("text"));
				writer.closeChunk();
			}
			else if ( c->getName() == "image" ) {
				Rect rect;
				c->getRect("rect",&rect);
				writer.startChunk(4,1);
				writer.write(id);
				writer.write(pos);
				writer.write(clr);
				writer.write(scale);
				writer.write(rect);
				writer.closeChunk();
			}
		}		
		writer.close();
	}
}

}