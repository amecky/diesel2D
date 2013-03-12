#pragma once
#include "..\utils\GameMath.h"
#include <vector>

namespace ds {
// -------------------------------------------------------
// TextureAtlas
//
// This class manages texture coordinates for given
// names.
// -------------------------------------------------------
class TextureAtlas {

struct TAItem {
	IdString hashName;
	Rect textureRect;
};

typedef std::vector<TAItem> Items;

public:
	TextureAtlas() : m_GridSize(0) , m_TextureSize(1024) {}
	TextureAtlas(int gridSize,int textureSize);
	~TextureAtlas(void);
	void addItem(const char* name,const Vec2& gridPos,const Vec2& size);
	void addItem(const char* name,const Rect& rect);
	bool getTextureCoordinates(const char* itemName,float *u1,float* v1,float* u2,float* v2);
	const float getWidth(const char* name) const;
	const float getHeight(const char* name) const;
	void setGridSize(int gs) {
		m_GridSize = gs;
	}
	void setTextureSize(int ts) {
		m_TextureSize = ts;
	}
private:
	int m_GridSize;
	int m_TextureSize;
	Items m_Items;
	bool m_UseHalfTexel;
};

}
