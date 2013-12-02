#include "TextEntity.h"
#include "..\renderer\VertexDeclaration.h"
#include "..\utils\Profiler.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"

namespace ds {

TextEntity::TextEntity() : Entity() , m_TextCounter(0) , m_Total(0) {	
}

void TextEntity::init(Renderer* renderer,SpriteBatch* spriteBatch,const char* fontName,int textureID,int batchID) {
	m_Batch = spriteBatch;
	font::load(fontName,renderer,textureID,m_BitmapFont);
	m_BatchID = batchID;
}

// -------------------------------------------------------
// Update text with int value
// -------------------------------------------------------
void TextEntity::updateIntText(uint32 id,int value,int length) {
	TextEntry* te = &m_TextEntries[id];
	if ( te != 0 ) {
		std::string	str = "";
		string::formatInt(value,str,length);		
		te->text = str;
		createText(te,str);
	}
}

// ----------------------------------------------------------
//
// ----------------------------------------------------------
Vec2 TextEntity::getDimension(uint32 id) {
	TextEntry* te = &m_TextEntries[id];
	return font::calculateSize(m_BitmapFont,te->text,te->scale);	
}

// ----------------------------------------------------------
// Calculate size
// ----------------------------------------------------------
Vec2 TextEntity::calculateSize(const std::string& text,float scale) {
	return font::calculateSize(m_BitmapFont,text,scale);	
}
// -------------------------------------------------------
// Creates vertices for text entry
// -------------------------------------------------------
void TextEntity::createText(TextEntry* textEntry,const std::string& text) {
	textEntry->sprites.clear();
	font::createText(m_BitmapFont,textEntry->pos,text,textEntry->color,textEntry->sprites,textEntry->scale,textEntry->scale);	
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void TextEntity::updateText(uint32 id,const std::string& txt) {
	TextEntry* te = &m_TextEntries[id];
	if ( te != 0 ) {
		te->text = txt;
		createText(te,txt);
	}
}
// -------------------------------------------------------
// Add text entry
// -------------------------------------------------------
uint32 TextEntity::addText(int x,int y,const std::string& txt,const Color& color,float scale) {
	TextEntry te;
	te.pos = Vec2(x,y);
	te.id = m_TextCounter;
	te.text = txt;
	te.scale = scale;
	te.active = true;
	te.color = color;
	m_TextEntries.push_back(te);
	++m_TextCounter;
	updateText(te.id,txt);
	return te.id;
}

// -------------------------------------------------------
// Set color
// -------------------------------------------------------
void TextEntity::setColor(uint32 id,const Color& color) {
	TextEntry* te = &m_TextEntries[id];
	for ( size_t i = 0; i < te->sprites.size(); ++i ) {
		te->sprites[i].color = color;
	}
}
// -------------------------------------------------------
// Render
// -------------------------------------------------------
void TextEntity::render() {
	for ( size_t i = 0; i < m_TextEntries.size(); ++i ) {
		TextEntry* te = &m_TextEntries[i];
		if ( te->active ) {
			for ( size_t j = 0; j < te->sprites.size(); ++j ) {
				m_Batch->draw(te->sprites[j]);
			}
		}
	}
}

};
