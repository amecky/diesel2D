#pragma once
#include "..\math\math_types.h"
#include <vector>
#include "..\renderer\render_types.h"
#include "..\sprites\Sprite.h"
#include "..\base\GameObject.h"
#include "..\renderer\BitmapFont.h"

namespace ds {

// -------------------------------------------------------
// TextEntry
// -------------------------------------------------------
struct TextEntry {
	uint32 id;
	std::string text;
	Vector2f pos;
	Color color;
	float scale;
	bool active;
	std::vector<Sprite> sprites;
};

// -------------------------------------------------------
// TextEntity
// -------------------------------------------------------
class TextMessage : public GameObject {

typedef std::vector<TextEntry> TextEntries;

public:
	TextMessage();
	virtual ~TextMessage() {}
	void init() {}
	void init(const char* fontName,int textureID);

	const TextEntry& getText(int index) const {
		return m_TextEntries[index];
	}
	void update(float elapsed) {}
	void updateText(uint32 id,const std::string& txt);
	void updateIntText(uint32 id,int value,int length);
	void setText(uint32 index,const Vector2f& pos,const std::string& text,const Color& color = Color::WHITE,float scale = 1.0f) {
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
	void setPosition(uint32 id,const Vector2f& pos) {
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
	Vector2f getDimension(uint32 id);
	Vector2f calculateSize(const std::string& text,float scale = 1.0f);

	void render();

private:		
	void createText(TextEntry* textEntry,const std::string& text);
	int m_Font;
	int m_Total;
	uint32 m_TextCounter;
	TextEntries m_TextEntries;
	BitmapFont* m_BitmapFont;

};

};

