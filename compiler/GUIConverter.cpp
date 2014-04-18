#include "GUIConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

GUIConverter::GUIConverter(void) : Converter("GUIConverter") {
}


GUIConverter::~GUIConverter(void) {
}

void GUIConverter::convert(const char* fileName) {
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
			if ( c->getName() == "gui" ) {
				writer.startChunk(CHNK_GUI,1);
				int textureID = c->getInt(0,"texture_id");
				std::string font = c->getProperty("font");
				int maxQuads = 1024;
				c->getInt("max_quads",&maxQuads);
				writer.write(textureID);
				writer.write(font);
				writer.write(maxQuads);
				writer.closeChunk();
			}
			else {
				writer.startChunk(CHNK_DLG_DEF,1);
				writer.write(c->getName());
				int id = 0;
				c->getInt("id",&id);
				writer.write(id);
				writer.write(c->getProperty("file"));
				writer.closeChunk();
			}			
		}		
		writer.close();
	}
}

}

