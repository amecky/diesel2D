#include "BitmapFont.h"
#include "..\io\json.h"

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
		/*
		cd.u1 = (float)cd.startX / textureSize;
		cd.v1 = (float)cd.startY / textureSize;
		cd.u2 = cd.u1 + (float)cd.width / (textureSize);
		cd.v2 = cd.v1 + ((float)charHeight) / (textureSize);
		cd.texureRect = Rect(static_cast<float>(startY), static_cast<float>(startX), static_cast<float>(width), static_cast<float>(charHeight));
		*/
		cd.texture = math::buildTexture(startY, startX, width, charHeight, textureSize, textureSize);
		definitions.append(cd);
	}

	bool BitmapFont::saveData(JSONWriter& writer) {
		return true;
	}

	bool BitmapFont::loadData(const JSONReader& loader) {
		int cats[32];
		int num = loader.get_categories(cats, 32);
		for (int i = 0; i < num; ++i) {
			if (strcmp(loader.get_category_name(cats[i]),"font") == 0 ) {
				loader.get_int(cats[i], "start_char", &startChar);
				loader.get_int(cats[i], "end_char", &endChar);
				loader.get_int(cats[i], "char_height", &charHeight);
				loader.get_int(cats[i], "grid_height", &gridHeight);
				loader.get_int(cats[i], "start_x", &startX);
				loader.get_int(cats[i], "start_y", &startY);
				loader.get_int(cats[i], "width", &width);
				loader.get_int(cats[i], "height", &height);
				loader.get_int(cats[i], "padding", &padding);
				loader.get_int(cats[i], "texture_size", &textureSize);
			}
		}
		return true;
	}
	
	const bool BitmapFont::contains(char c) const {
		int idx = (int)c - startChar;
		return idx >= 0 && idx < definitions.size();
	}

	const CharDef& BitmapFont::getCharDef(char c) const {
		return definitions[(int)c - startChar];
	}

}
