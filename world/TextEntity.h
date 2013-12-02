#pragma once
#include "..\math\math_types.h"
#include <vector>
#include "..\renderer\render_types.h"
#include "..\nodes\SpriteBatch.h"
#include "World.h"

namespace ds {

// -------------------------------------------------------
// TextEntry
// -------------------------------------------------------
struct TextEntry {
	uint32 id;
	std::string text;
	Vec2 pos;
	Color color;
	float scale;
	bool active;
	std::vector<Sprite> sprites;
};

// -------------------------------------------------------
// TextEntity
// -------------------------------------------------------
class TextEntity : public Entity {

typedef std::vector<TextEntry> TextEntries;

public:
	TextEntity();
	virtual ~TextEntity() {}
	void init(Renderer* renderer,SpriteBatch* spriteBatch,const char* fontName,int textureID,int batchID);

	const TextEntry& getText(int index) const {
		return m_TextEntries[index];
	}
	void update(float elapsed) {}
	void updateText(uint32 id,const std::string& txt);
	void updateIntText(uint32 id,int value,int length);
	void setText(uint32 index,const Vec2& pos,const std::string& text,const Color& color = Color::WHITE,float scale = 1.0f) {
		TextEntry te;
		te.pos = pos;
		te.id = index;
		te.text = text;
		te.scale = scale;
		te.color = color;
		te.active = true;
		createText(&te,text);
		m_TextEntries[index] = te;
	}
	uint32 addText(int x,int y,const std::string& txt,const Color& color = Color::WHITE,float scale = 1.0f);
	void setColor(uint32 id,const Color& color);
	void setScale(uint32 id,float scale) {
		TextEntry* te = &m_TextEntries[id];
		if ( te != 0 ) {
			te->scale = scale;
			createText(te,te->text);
		}
	}
	void setPosition(uint32 id,const ds::Vec2& pos) {
		TextEntry* te = &m_TextEntries[id];
		if ( te != 0 ) {
			te->pos = pos;
			createText(te,te->text);
		}
	}
	void setState(uint32 id,bool active) {
		TextEntry* te = &m_TextEntries[id];
		te->active = active;
	}
	Vec2 getDimension(uint32 id);
	Vec2 calculateSize(const std::string& text,float scale = 1.0f);

	void render();

	const EntityType getType() const {
		return ET_TEXT;
	}
	const int getBatchItemID() const {
		return m_BatchID;
	}
private:		
	void createText(TextEntry* textEntry,const std::string& text);
	int m_Font;
	int m_Total;
	uint32 m_TextCounter;
	TextEntries m_TextEntries;
	SpriteBatch* m_Batch;
	BitmapFont m_BitmapFont;
	int m_BatchID;
};

};

