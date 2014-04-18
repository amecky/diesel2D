#include "BitmapFontConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

void BitmapFontConverter::convert(const char* fileName) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",fileName);
	char out[256];
	sprintf(out,"data\\%s.dsb",fileName);
	BinaryWriter writer;
	int signature[] = {0,8,15};
	writer.open(out,signature,3);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		Category* c = reader.getCategory("font");
		writer.startChunk(CHNK_FONT,1);
		writer.write(c->getInt(0,"start_char"));
		writer.write(c->getInt(0,"end_char"));
		writer.write(c->getInt(0,"char_height"));
		writer.write(c->getInt(0,"grid_height"));
		writer.write(c->getInt(0,"start_x"));
		writer.write(c->getInt(0,"start_y"));
		writer.write(c->getInt(0,"width"));
		writer.write(c->getInt(0,"height"));
		writer.write(c->getInt(0,"padding"));
		writer.write(c->getInt(0,"texture_size"));	
		writer.closeChunk();
		writer.close();
	}
}

}