#include "TextCounter.h"
#include "..\utils\StringUtils.h"

namespace ds {

TextCounter::TextCounter(TextNode* textNode,const Vec2& position,int digits,const Color& color,float scale) 
	: m_TextNode(textNode) , m_Active(false) , m_Digits(digits) , m_Timer(0.0f) , m_Value(0) {
	std::string txt;
	string::formatInt(0,txt,digits);
	m_ValueIdx = m_TextNode->addText(position,txt,color,scale);
	m_ScaleTimer = 0.0f;
	m_ScaleTTL = 1.0f;
	m_Scaling = false;
	m_ScalueMultiplier = 1.0f;
	m_ScaleStart = 1.0f;
	reset();
}

TextCounter::~TextCounter(void) {
}

void TextCounter::reset() {
	m_Value = 0;
	m_ScaleTimer = 0.0f;
}

void TextCounter::add(int value) {
	m_Value += value;
	if ( value != 0 ) {
		std::string str;
		string::formatInt(m_Value,str,m_Digits);
		m_TextNode->updateText(m_ValueIdx,str);
	}
}

void TextCounter::set(int value) {
	m_Value = value;
	std::string str;
	string::formatInt(m_Value,str,m_Digits);
	m_TextNode->updateText(m_ValueIdx,str);
}

void TextCounter::enableAutoScaling(float startScale,float scaleMultiplier,float ttl) {
	m_ScaleStart = startScale;
	m_ScalueMultiplier = scaleMultiplier;
	m_ScaleTTL = ttl;
}

void TextCounter::startScaling() {
	m_Scaling = true;
	m_ScaleTimer = 0.0f;
}

void TextCounter::update(float elapsed) {
	if ( m_Scaling ) {
		m_ScaleTimer += elapsed * m_ScalueMultiplier;
		if ( m_ScaleTimer > m_ScaleTTL ) {
			m_Scaling = false;
			m_TextNode->setScale(m_ValueIdx,2.0f);
		}
		float t = m_ScaleStart - math::smoothstep(m_ScaleTimer/m_ScaleStart) * 0.2f;
		m_TextNode->setScale(m_ValueIdx,t);
	}
}

}