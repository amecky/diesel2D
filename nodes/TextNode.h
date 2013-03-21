#pragma once
#include "..\math\math_types.h"
#include <vector>
#include "..\nodes\Node.h"
#include "..\renderer\render_types.h"
#include "SimpleQuadNode.h"

namespace ds {

struct TextEntry {
	uint32 id;
	std::string text;
	Vec2 pos;
	Color color;
	float scale;
	bool active;
	std::vector<uint32> vertices;
};

class TextNode : public SimpleQuadNode {

typedef std::map<uint32,TextEntry> TextEntries;
typedef std::map<uint32,TextEntry>::iterator TextEntriesIterator;

public:
	TextNode(const char* nodeName,Renderer* renderer,int material,int bitmapFont);
	virtual ~TextNode() {}
	TextEntry& getText(int index) {
		return m_TextEntries[index];
	}
	void updateText(uint32 id,const std::string& txt);
	void updateIntText(uint32 id,int value,int length);
	void setText(uint32 index,const Vec2& pos,const Color& color,const std::string& text,float scale = 1.0f) {
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
	uint32 addText(const Vec2& pos,const std::string& txt,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	void setColor(uint32 id,const Color& color) {
		TextEntry* te = &m_TextEntries[id];
		if ( te != 0 ) {
			te->color = color;
			createText(te,te->text);
		}
	}
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
	Vec2 getDimension(uint32 id);
	void setState(uint32 id,bool active) {
		TextEntry* te = &m_TextEntries[id];
		if ( te != 0 ) {
			te->active = active;
		}
	}
	Vec2 calculateSize(const std::string& text,float scale = 1.0f);
private:		
	void createText(TextEntry* textEntry,const std::string& text);
	int m_Font;
	int m_Total;
	uint32 m_TextCounter;
	TextEntries m_TextEntries;
};

};

