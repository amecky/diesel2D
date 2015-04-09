#include "DialogConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

void DialogConverter::convert(JSONReader& reader,BinaryWriter& writer) {
	std::vector<Category*> categories = reader.getCategories();
	for ( size_t i = 0; i < categories.size(); ++i ) {
		Category* c = categories[i];
		if ( c->getName() == "image" ) {
			int id = c->getInt(0, "id");
			Rect r;
			c->getRect("rect",&r);
			Vector2f p = c->getVector2f("pos");
			bool centered = true;
			c->getBool("centered",&centered);
			writer.startChunk(CHNK_DLG_IMAGE,1);
			writer.write(id);
			writer.write(r);
			writer.write(p);
			int cnt = 0;
			if ( centered ) {
				cnt = 1;
			}
			writer.write(cnt);
			writer.closeChunk();				
		}
		if (c->getName() == "image_link") {
			int id = c->getInt(0, "id");
			Rect r;
			c->getRect("rect", &r);
			Vector2f p = c->getVector2f("pos");
			bool centered = true;
			c->getBool("centered", &centered);
			writer.startChunk(CHNK_DLG_IMAGE_LINK, 1);
			writer.write(id);
			writer.write(r);
			writer.write(p);
			int cnt = 0;
			if (centered) {
				cnt = 1;
			}
			writer.write(cnt);
			writer.closeChunk();
		}
		if ( c->getName() == "button" ) {
			int id = c->getInt(0,"id");
			Rect r;
			c->getRect("rect",&r);
			Vector2f p = c->getVector2f("pos");
			std::string txt = c->getProperty("text");
			Color clr = Color::WHITE;
			c->getColor("text_color",&clr);
			bool centered = true;
			c->getBool("centered", &centered);
			writer.startChunk(CHNK_DLG_BUTTON,1);
			writer.write(id);
			writer.write(r);
			writer.write(p);
			writer.write(txt);				
			writer.write(clr);
			int cnt = 0;
			if (centered) {
				cnt = 1;
			}
			writer.write(cnt);
			writer.closeChunk();	
		}
		if ( c->getName() == "text" ) {
			int id = c->getInt(0,"id");
			Vector2f p = c->getVector2f("pos");
			float scale = 1.0f;
			c->getFloat("scale",&scale);
			std::string txt = c->getProperty("text");
			Color clr = Color::WHITE;
			c->getColor("color",&clr);
			bool centered = true;
			c->getBool("centered",&centered);
			int cnt = 0;
			if ( centered ) {
				cnt = 1;
			}
			writer.startChunk(CHNK_DLG_TEXT,1);
			writer.write(id);
			writer.write(p);
			writer.write(scale);				
			writer.write(txt);	
			writer.write(clr);
			writer.write(cnt);
			writer.closeChunk();	
		}
	}				
}

}
