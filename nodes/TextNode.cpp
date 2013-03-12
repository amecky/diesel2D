#include "TextNode.h"
#include "..\renderer\VertexDeclaration.h"
#include "..\utils\Profiler.h"
#include "..\utils\StringUtils.h"

namespace ds {

TextNode::TextNode(const char* nodeName,Renderer* renderer,int material,int bitmapFont) 
	: SimpleQuadNode(nodeName,renderer,material,400) , m_Font(bitmapFont) , m_TextCounter(0) , m_Total(0) {	
}

// -------------------------------------------------------
// Update text with int value
// -------------------------------------------------------
void TextNode::updateIntText(uint32 id,int value,int length) {
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
Vec2 TextNode::getDimension(uint32 id) {
	Vec2 ret(0,0);
	TextEntry* te = &m_TextEntries[id];
	float dimX = 0.0f;
	float dimY = 0.0f;
	float padding = 0.0f;
	float x = 10.0f;
	if ( te != 0 ) {
		std::string text = te->text;
		float scale = te->scale;
		for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
			char c = text[cnt];
			CharDef cd = m_Renderer->getCharDef(m_Font,c);
			padding = (cd.width+2)  * scale;
			dimX += cd.width * scale + 0.5f;
			float dy = m_Renderer->getCharHeight(m_Font) * scale + 0.5f;
			if ( dy > dimY ) {
				dimY = dy;
			}
		}
		ret.x = dimX;
		ret.y = dimY;
	}
	return ret;
}

// ----------------------------------------------------------
// Calculate size
// ----------------------------------------------------------
Vec2 TextNode::calculateSize(const std::string& text,float scale) {
	Vec2 ret(0,0);	
	float dimX = 0.0f;
	float dimY = 0.0f;
	float padding = 0.0f;
	float x = 0.0f;
	for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
		char c = text[cnt];
		CharDef cd = m_Renderer->getCharDef(m_Font,c);	
		dimX += cd.width * scale + 2.0f;
		x += padding;
		padding = (cd.width+2)  * scale;
		float dy = m_Renderer->getCharHeight(m_Font) * scale + 0.5f;
		if ( dy > dimY ) {
			dimY = dy;
		}
	}
	ret.x = dimX;
	ret.y = dimY;
	return ret;
}
// -------------------------------------------------------
// Creates vertices for text entry
// -------------------------------------------------------
void TextNode::createText(TextEntry* textEntry,const std::string& text) {
	for ( size_t i = 0; i < textEntry->vertices.size(); ++i ) {
		uint32 idx = textEntry->vertices[i];
		remove(idx);
	}
	textEntry->vertices.clear();
	float x = textEntry->pos.x - 0.5f;
	float y = textEntry->pos.y - 0.5f;
	float padding = 0.0f;
	float z = 0.0f;
	float scale = textEntry->scale;
	for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
		char c = text[cnt];
		CharDef cd = m_Renderer->getCharDef(m_Font,c);
		x += padding;
		padding = (cd.width+2)  * scale;
		float dimX = cd.width * scale + 0.5f;
		float dimY = m_Renderer->getCharHeight(m_Font) * scale + 0.5f;
		uint32 idx = create(dimX,dimY,Vec2(x,y),cd.u1,cd.v1,cd.u2,cd.v2,textEntry->color);
		textEntry->vertices.push_back(idx);		
	}
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void TextNode::updateText(uint32 id,const std::string& txt) {
	TextEntry* te = &m_TextEntries[id];
	if ( te != 0 ) {
		te->text = txt;
		createText(te,txt);
	}
}
// -------------------------------------------------------
// Add text entry
// -------------------------------------------------------
uint32 TextNode::addText(const Vec2& pos,const std::string& txt,const Color& color,float scale) {
	TextEntry te;
	te.pos = pos;
	te.id = m_TextCounter;
	te.text = txt;
	te.scale = scale;
	te.active = true;
	te.color = color;
	m_TextEntries[m_TextCounter] = te;
	++m_TextCounter;
	updateText(te.id,txt);
	return te.id;
}
};
