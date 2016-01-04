#pragma once
#include "..\utils\StringUtils.h"
#include "..\lib\container\List.h"
#include "render_types.h"

namespace ds {

struct FontDefinition {
	int startChar;
	int endChar;
	int width;
	int height;
	int padding;
	int textureSize;
	int charHeight;
	int startX;
	int startY;
	int gridHeight;
};
// -------------------------------------------------------
// Character definition
// -------------------------------------------------------
struct CharDef {
	uint32 ascii;
	int startX;
	int startY;
	int width;
	float u1;
	float u2;
	float v1;
	float v2;
	Rect texureRect;
};

struct BitmapFont {

	char name[32];
	IdString hashName;
	int startChar;
	int endChar;
	int width;
	int height;
	int padding;
	int textureSize;
	int charHeight;
	int startX;
	int startY;
	int gridHeight;
	int textureID;
	List<CharDef> definitions;

	BitmapFont(const char* fontName,int texID) {
		sprintf_s(name, 32, "%s", fontName);
		hashName = string::murmur_hash(fontName);
		textureID = texID;
	}

	~BitmapFont() {}

	void intialize(const FontDefinition& definition);

	void addChar(uint32 ascii, int startX, int startY, int width);

	//bool load();

	//void save();

	void importJSON();

	void exportJSON();

	const bool contains(char c) const;

	const CharDef& getCharDef(char c) const;
	
};

}

