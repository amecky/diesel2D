#pragma once
#include "..\io\Serializer.h"
#include "..\lib\container\List.h"

namespace ds {

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

class BitmapFont : public Serializer {

public:
	BitmapFont() : hashName(0) {}
	~BitmapFont() {}
	void addChar(uint32 ascii,int startX,int startY,int width) {
		CharDef cd;
		cd.ascii = ascii;
		cd.startX = startX;
		cd.startY = startY;
		cd.width = width;
		cd.u1 = (float)cd.startX/textureSize;
		cd.v1 = (float)cd.startY/textureSize;
		cd.u2 = cd.u1 + (float)cd.width/(textureSize);
		cd.v2 = cd.v1 + ((float)charHeight)/(textureSize);
		cd.texureRect = Rect(static_cast<float>(startY),static_cast<float>(startX),static_cast<float>(width),static_cast<float>(charHeight));
		definitions.append(cd);
	}
	const bool contains(char c) const {
		int idx = (int)c - startChar;
		return idx >= 0 && idx < definitions.size();
	}
	const CharDef& getCharDef(char c) const {
		return definitions[(int)c-startChar];
	}
	void load(BinaryLoader* loader);
	const IdString getHashName() const {
		return hashName;
	}
	const int getStartChar() const {
		return startChar;
	}
	const int getEndChar() const {
		return endChar;
	}
	const int getWidth() const {
		return width;
	}
	const int getHeight() const {
		return height;
	}
	const int getPadding() const {
	return padding;
	}
	const int getTextureSize() const {
	return textureSize;
	}
	const int getCharHeight() const {
	return charHeight;
	}
	const int getStartX() const {
	return startX;
	}
	const int getStartY() const {
	return startY;
	}
	const int getGridHeight() const {
		return gridHeight;		
	}
	void setHashName(IdString hash) {
		hashName = hash;
	}
private:
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
	List<CharDef> definitions;
};

}

