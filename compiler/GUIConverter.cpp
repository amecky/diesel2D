#include "GUIConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

GUIConverter::GUIConverter(void) : Converter("GUIConverter") {
}


GUIConverter::~GUIConverter(void) {
}

void GUIConverter::convert(JSONReader& reader,BinaryWriter& writer) {	
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
		
}

}

