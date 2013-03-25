#include "TextureAtlas.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"

namespace ds {

namespace atlas {

	// -------------------------------------------------------
	// Add entry to texture atlas
	// -------------------------------------------------------
	void add(TAtlas& atlas,const char* name,Rect textureRect,bool useHalfTexel) {
		AtlasEntry ae;
		ae.hash = string::murmur_hash(name);
		ae.rect = textureRect;
		float texSize = atlas.textureSize;
		if ( useHalfTexel ) {
			float halfTexel = 0.5f;

			float kUOffset = halfTexel / texSize;
			float kVOffset = halfTexel / texSize;

			ae.uv[0] = static_cast<float>(textureRect.left) / texSize + kUOffset;
			ae.uv[1] = static_cast<float>(textureRect.top) / texSize + kVOffset;  

			ae.uv[2] = ae.uv[0] + static_cast<float>(textureRect.width()) / texSize - 2.0f*kUOffset;
			ae.uv[3] = ae.uv[1] + static_cast<float>(textureRect.height()) / texSize  - 2.0f*kVOffset;			

		}
		else {
			ae.uv[0] = static_cast<float>(textureRect.left) / texSize;
			ae.uv[2] = static_cast<float>(textureRect.right) / texSize;
			ae.uv[1] = static_cast<float>(textureRect.top) / texSize;
			ae.uv[3] = static_cast<float>(textureRect.bottom) / texSize;
		}				
		atlas.entries.append(ae);
	}

	// -------------------------------------------------------
	// Find entry in texture atlas
	// -------------------------------------------------------
	bool findEntry(const TAtlas& atlas,const char* name,AtlasEntry* entry) {
		IdString hash = string::murmur_hash(name);
		for ( uint32 i = 0; i < atlas.entries.num(); ++i ) {
			if ( atlas.entries[i].hash == hash ) {				
				entry->hash = atlas.entries[i].hash;
				entry->rect = atlas.entries[i].rect;				
				for ( int j = 0; j < 4; ++j ) {
					entry->uv[j] = atlas.entries[i].uv[j];
				}
				return true;
			}
		}
		return false;
	}

}

TextureAtlas::TextureAtlas(int gridSize,int textureSize) 
	: m_GridSize(gridSize) , m_TextureSize(textureSize) , m_UseHalfTexel(true) {
}


TextureAtlas::~TextureAtlas(void) {
}

// -------------------------------------------------------
// Add item by grid pos and size
// -------------------------------------------------------
void TextureAtlas::addItem(const char* name,const Vec2& gridPos,const Vec2& size) {
	TAItem item;
	item.hashName = string::murmur_hash(name);
	float top = gridPos.x * m_GridSize;
	float left = gridPos.y * m_GridSize;
	item.textureRect = Rect(left,top,size.x,size.y);
	LOG(logINFO) << "adding item " << name << " rect " << item.textureRect.top << "," << item.textureRect.left << "," << item.textureRect.width() << "," << item.textureRect.height();
	m_Items.push_back(item);
}

// -------------------------------------------------------
// Add item with rect
// -------------------------------------------------------
void TextureAtlas::addItem(const char* name,const Rect& rect) {
	TAItem item;
	item.hashName = string::murmur_hash(name);	
	item.textureRect = rect;
	LOG(logINFO) << "adding item " << name << " rect " << item.textureRect.top << "," << item.textureRect.left << "," << item.textureRect.width() << "," << item.textureRect.height();
	m_Items.push_back(item);
}

// -------------------------------------------------------
// Get width of specific item
// -------------------------------------------------------
const float TextureAtlas::getWidth(const char* name) const {
	IdString hash = string::murmur_hash(name);
	for ( size_t i = 0; i < m_Items.size(); ++i ) {
		const TAItem* item = &m_Items[i];
		if ( item->hashName == hash ) {
			return static_cast<float>(item->textureRect.width());
		}
	}
	return 0.0f;
}

// -------------------------------------------------------
// Get height of specific item
// -------------------------------------------------------
const float TextureAtlas::getHeight(const char* name) const {
	IdString hash = string::murmur_hash(name);
	for ( size_t i = 0; i < m_Items.size(); ++i ) {
		const TAItem* item = &m_Items[i];
		if ( item->hashName == hash ) {
			return static_cast<float>(item->textureRect.height());
		}
	}
	return 0.0f;
}

// -------------------------------------------------------
// Get texture coordinates for specific item
// -------------------------------------------------------
bool TextureAtlas::getTextureCoordinates(const char* itemName,float *u1,float* v1,float* u2,float* v2) {
	IdString hash = string::murmur_hash(itemName);
	float texSize = static_cast<float>(m_TextureSize);
	for ( size_t i = 0; i < m_Items.size(); ++i ) {
		TAItem* item = &m_Items[i];
		if ( item->hashName == hash ) {
			Rect textureRect = item->textureRect;
			if ( m_UseHalfTexel ) {
				float halfTexel = 0.5f;

				float kUOffset = halfTexel / texSize;
				float kVOffset = halfTexel / texSize;

				*u1 = static_cast<float>(textureRect.left) / texSize + kUOffset;
				*v1 = static_cast<float>(textureRect.top) / texSize + kVOffset;  

				*u2 = *u1 + static_cast<float>(textureRect.width()) / texSize - 2.0f*kUOffset;
				*v2 = *v1 + static_cast<float>(textureRect.height()) / texSize  - 2.0f*kVOffset;
			}
			else {
				*u1 = static_cast<float>(textureRect.left) / texSize;
				*u2 = static_cast<float>(textureRect.right) / texSize;
				*v1 = static_cast<float>(textureRect.top) / texSize;
				*v2 = static_cast<float>(textureRect.bottom) / texSize;
			}
			return true;
		}
	}
	return false;
}

}
