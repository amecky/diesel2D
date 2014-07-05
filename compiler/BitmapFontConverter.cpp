#include "BitmapFontConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

void BitmapFontConverter::convert(JSONReader& reader,BinaryWriter& writer) {
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
}

}