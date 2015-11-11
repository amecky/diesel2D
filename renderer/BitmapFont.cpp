#include "BitmapFont.h"
#include "..\utils\JSONWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryLoader.h"
#include "..\io\BinaryWriter.h"

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
		LOGC("BitmapFont") << "exporting to: << " << buffer;
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
		LOGC("BitmapFont") << "importing file: " << buffer;
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
			LOGC("BitmapFont") << "Cannot parse json file";
		}
	}

	// -------------------------------------------------------
	// load
	// -------------------------------------------------------
	bool BitmapFont::load() {
		BinaryLoader loader;
		char buffer[64];
		sprintf(buffer, "assets\\%u", hashName);
		LOGC("BitmapFont") << "loading file: " << buffer;
		int signature[] = { 0, 8, 15 };
		if (loader.open(buffer, signature, 3) == IO_OK) {
			while (loader.openChunk() == 0) {
				if (loader.getChunkID() == 1) {
					loader.read(&startChar);
					loader.read(&endChar);
					loader.read(&charHeight);
					loader.read(&gridHeight);
					loader.read(&startX);
					loader.read(&startY);
					loader.read(&width);
					loader.read(&height);
					loader.read(&padding);
					loader.read(&textureSize);
					loader.closeChunk();
					return true;
				}
			}
		}
		else {
			LOGC("BitmapFont") << "Error loading file";
		}
		return false;
	}

	// -------------------------------------------------------
	// save
	// -------------------------------------------------------
	void BitmapFont::save() {
		char buffer[64];
		sprintf(buffer, "assets\\%u", hashName);
		LOGC("BitmapFont") << "saving file: " << buffer;
		BinaryWriter writer;
		int signature[] = { 0, 8, 15 };
		writer.open(buffer, signature, 3);
		writer.startChunk(1, 1);
		writer.write(startChar);
		writer.write(endChar);
		writer.write(charHeight);
		writer.write(gridHeight);
		writer.write(startX);
		writer.write(startY);
		writer.write(width);
		writer.write(height);
		writer.write(padding);
		writer.write(textureSize);
		writer.closeChunk();		
		writer.close();
	}
	
	const bool BitmapFont::contains(char c) const {
		int idx = (int)c - startChar;
		return idx >= 0 && idx < definitions.size();
	}

	const CharDef& BitmapFont::getCharDef(char c) const {
		return definitions[(int)c - startChar];
	}

}
