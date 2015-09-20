#include "TextMessage.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\Log.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"

namespace ds {

TextMessage::TextMessage() : m_TextCounter(0) , m_Total(0) {	
}

void TextMessage::init(const char* fontName,int textureID) {
	m_BitmapFont = renderer::loadBitmapFont(fontName,textureID);
	assert(m_BitmapFont != 0);
	//font::load(fontName,m_Renderer,textureID,m_BitmapFont);
}

// -------------------------------------------------------
// Update text with int value
// -------------------------------------------------------
void TextMessage::updateIntText(uint32 id,int value,int length) {
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
Vector2f TextMessage::getDimension(uint32 id) {
	TextEntry* te = &m_TextEntries[id];
	return font::calculateSize(*m_BitmapFont,te->text,te->scale);	
}

// ----------------------------------------------------------
// Calculate size
// ----------------------------------------------------------
Vector2f TextMessage::calculateSize(const std::string& text,float scale) {
	return font::calculateSize(*m_BitmapFont,text,4,scale,scale);	
}
// -------------------------------------------------------
// Creates vertices for text entry
// -------------------------------------------------------
void TextMessage::createText(TextEntry* textEntry,const std::string& text) {
	textEntry->sprites.clear();
	font::createText(*m_BitmapFont,textEntry->pos,text,textEntry->color,textEntry->sprites,textEntry->scale,textEntry->scale);	
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void TextMessage::updateText(uint32 id,const std::string& txt) {
	TextEntry* te = &m_TextEntries[id];
	if ( te != 0 ) {
		te->text = txt;
		createText(te,txt);
	}
}
// -------------------------------------------------------
// Add text entry
// -------------------------------------------------------
uint32 TextMessage::addText(int x,int y,const std::string& txt,const Color& color,float scale) {
	TextEntry te;
	te.pos = Vector2f(x,y);
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
void TextMessage::setColor(uint32 id,const Color& color) {
	TextEntry* te = &m_TextEntries[id];
	assert(te != 0);	
	for ( size_t i = 0; i < te->sprites.size(); ++i ) {
		te->sprites[i].color = color;
	}
}
// -------------------------------------------------------
// Render
// -------------------------------------------------------
void TextMessage::render() {
	for ( size_t i = 0; i < m_TextEntries.size(); ++i ) {
		TextEntry* te = &m_TextEntries[i];
		if ( te->active ) {
			for ( size_t j = 0; j < te->sprites.size(); ++j ) {
				sprites::draw(te->sprites[j]);
			}
		}
	}
}

};
