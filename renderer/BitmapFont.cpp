#include "BitmapFont.h"
#include "..\utils\JSONWriter.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

	void BitmapFont::intialize(const FontDefinition& definition) {
		startChar = definition.startChar;
		endChar = definition.endChar;
		width = definition.width;
		height = definition.height;
		padding = definition.padding;
		textureSize = definition.textureSize;
		charHeight = definition.charHeight;
		startX = definition.startX;
		startY = definition.startY;
		gridHeight = definition.gridHeight;
	}

	void BitmapFont::addChar(uint32 ascii, int startX, int startY, int width) {
		CharDef cd;
		cd.ascii = ascii;
		cd.startX = startX;
		cd.startY = startY;
		cd.width = width;
		cd.u1 = (float)cd.startX / textureSize;
		cd.v1 = (float)cd.startY / textureSize;
		cd.u2 = cd.u1 + (float)cd.width / (textureSize);
		cd.v2 = cd.v1 + ((float)charHeight) / (textureSize);
		cd.texureRect = Rect(static_cast<float>(startY), static_cast<float>(startX), static_cast<float>(width), static_cast<float>(charHeight));
		definitions.append(cd);
	}

	// -------------------------------------------------------
	// export json
	// -------------------------------------------------------
	void BitmapFont::exportJSON() {
		char buffer[64];
		sprintf(buffer, "content\\resources\\%s.json",name);
		LOG << "exporting to: << " << buffer;
		JSONWriter jw;
		jw.open(buffer);
		jw.startCategory("font");
		jw.write("start_char",startChar);
		jw.write("end_char",endChar);
		jw.write("char_height",charHeight);
		jw.write("grid_height",gridHeight);
		jw.write("start_x",startX);
		jw.write("start_y",startY);
		jw.write("width",width);
		jw.write("height",height);
		jw.write("padding",padding);
		jw.write("texture_size",textureSize);
		jw.endCategory();
	}

	// -------------------------------------------------------
	// import json
	// -------------------------------------------------------
	void BitmapFont::importJSON() {
		char buffer[64];
		sprintf(buffer, "content\\resources\\%s.json", name);
		LOG << "importing file: " << buffer;
		JSONReader reader;
		if (reader.parse(buffer)) {
			Category* c = reader.getCategory("font");
			if (c != 0) {
				c->getInt("start_char", &startChar);
				c->getInt("end_char", &endChar);
				c->getInt("char_height", &charHeight);
				c->getInt("grid_height", &gridHeight);
				c->getInt("start_x", &startX);
				c->getInt("start_y", &startY);
				c->getInt("width", &width);
				c->getInt("height", &height);
				c->getInt("padding", &padding);
				c->getInt("texture_size", &textureSize);
			}
		}
		else {
			LOG << "Cannot parse json file";
		}
	}

	const bool BitmapFont::contains(char c) const {
		int idx = (int)c - startChar;
		return idx >= 0 && idx < definitions.size();
	}

	const CharDef& BitmapFont::getCharDef(char c) const {
		return definitions[(int)c - startChar];
	}

}
