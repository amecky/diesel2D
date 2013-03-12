#include "BaseHUD.h"
#include <math.h>
#include "..\utils\StringUtils.h"

namespace ds {
// ---------------------------------------------------------------
//  BaseHUD
// ---------------------------------------------------------------
BaseHUD::BaseHUD(ds::Renderer* renderer,int material,int bitmapFont) : m_Renderer(renderer) , m_Mtrl(material) , m_BitmapFont(bitmapFont) , m_Running(false) {
	m_Buffer = new ds::QuadBuffer(renderer,1024);
	for ( int i = 0; i < 128; ++i ) {
		m_HUDEntries[i].flag = -1;
	}
	for ( int i = 0 ; i < 16; ++i ) {
		m_Counter[i].flag = -1;
	}
	for ( int i = 0; i < 32; ++i ) {
		m_TextEntries[i].flag = -1;
	}
	for ( int i = 0; i < 16; ++i ) {
		m_Timer[i].flag = -1;
	}
}

// -------------------------------------------------------
// Create new HUD entry
// -------------------------------------------------------
int BaseHUD::createEntry(const ds::Vec2& pos,float scale,const ds::Color& color) {
	int id = -1;
	for ( int i = 0; i < 128; ++i ) {
		if ( m_HUDEntries[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		m_HUDEntries[id].color = color;
		m_HUDEntries[id].pos = pos;
		m_HUDEntries[id].flag = 0;
		m_HUDEntries[id].scale = scale;
	}
	return id;
}

// -------------------------------------------------------
// Add static image
// -------------------------------------------------------
void BaseHUD::addImage(const ds::Vec2& pos,const ds::Rect& textureRect,float textureSize) {
	m_Buffer->create(textureRect.width(),textureRect.height(),0.0f,pos,textureRect,textureSize);
}

// -------------------------------------------------------
// Add text
// -------------------------------------------------------
int BaseHUD::addText(const ds::Vec2& pos,const std::string& txt,const ds::Color& color,float scale) {
	int id = -1;
	for ( int i = 0; i < 32 ; ++i ) {
		if ( m_TextEntries[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		HUDText* ht = &m_TextEntries[id];
		ht->entryID = createEntry(pos,scale,color);		
		ht->text = txt;	
		HUDEntry* he = &m_HUDEntries[ht->entryID];
		createText(he,txt,false);
	}
	return id;
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void BaseHUD::updateText(int id,const std::string& text) {
	HUDText* ht = &m_TextEntries[id];
	ht->text = text;	
	HUDEntry* he = &m_HUDEntries[ht->entryID];
	createText(he,text,true);
}

// -------------------------------------------------------
// Add counter
// -------------------------------------------------------
int BaseHUD::addCounter(const ds::Vec2& pos,int length,int value,const ds::Color& color,float scale) {
	int id = -1;
	for ( int i = 0; i < 16; ++i ) {
		if ( m_Counter[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		HUDCounter* hc = &m_Counter[id];
		hc->entryID = createEntry(pos,scale,color);
		hc->id = id;
		hc->length = length;
		hc->value = value;		
		hc->flag = 0;		
		setCounterValue(id,value);
	}	
	return id;
}

// -------------------------------------------------------
// Add Timer
// -------------------------------------------------------
int BaseHUD::addTimer(const ds::Vec2& pos,const ds::Color& color,float scale) {
	int id = -1;
	for ( int i = 0; i < 16; ++i ) {
		if ( m_Timer[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		HUDTimer* timer = &m_Timer[id];
		timer->entryID = createEntry(pos,scale,color);
		timer->id = id;
		timer->minutes = 0;
		timer->seconds = 0;
		timer->mode = 1;
		timer->forward = true;
		timer->flag = 0;		
		timer->timer = 0.0f;
		setTimer(timer->id,0,0);
	}	
	return id;
}

// -------------------------------------------------------
// Tick all timer
// -------------------------------------------------------
void BaseHUD::tick(float elapsed) {
	for ( int i = 0; i < 16; ++i ) {
		if ( m_Timer[i].flag != -1 ) {
			HUDTimer* timer = &m_Timer[i];
			timer->timer += elapsed;
			if ( timer->timer >= 1.0f ) {
				timer->timer = 0.0f;
				++timer->seconds;
				if ( timer->seconds >= 60 ) {
					timer->seconds = 0;
					++timer->minutes;
				}
				setTimer(timer->id,timer->minutes,timer->seconds);
			}
		}
	}
}

// -------------------------------------------------------
// Set counter value
// -------------------------------------------------------
void BaseHUD::setTimer(int id,int minutes,int seconds) {
	assert(id >= 0 && id < 16);
	HUDTimer* hc = &m_Timer[id];
	hc->minutes = minutes;
	hc->seconds = seconds;
	std::string text;
	ds::string::formatTime(minutes,seconds,text);
	HUDEntry* entry = &m_HUDEntries[hc->entryID];
	createText(entry,text,true);
}

// -------------------------------------------------------
// Set counter value
// -------------------------------------------------------
void BaseHUD::setCounterValue(int id,int value) {
	assert(id >= 0 && id < 16);
	HUDCounter* hc = &m_Counter[id];
	hc->value = value;
	std::string text;
	ds::string::formatInt(value,text,hc->length);
	HUDEntry* entry = &m_HUDEntries[hc->entryID];
	createText(entry,text,true);
}

// -------------------------------------------------------
// Draw internal buffer
// -------------------------------------------------------
void BaseHUD::render() {
	m_Buffer->draw();
}

// -------------------------------------------------------
// Private methods
// -------------------------------------------------------

// -------------------------------------------------------
// Create text
// -------------------------------------------------------
void BaseHUD::createText(HUDEntry* entry,const std::string& text,bool clear) {
	if ( clear ) {
		for ( size_t i = 0; i < entry->vertices.size(); ++i ) {
			m_Buffer->remove(entry->vertices[i]);
		}
		entry->vertices.clear();
	}
	float x = entry->pos.x;
	float y = entry->pos.y;
	float padding = 0.0f;
	float z = 0.0f;
	for ( size_t cnt = 0; cnt < text.length(); ++cnt ) {
		char c = text[cnt];
		ds::CharDef cd = m_Renderer->getCharDef(m_BitmapFont,c);		
		padding = (cd.width+2)  * entry->scale;		
		float dimX = cd.width * entry->scale;
		float dimY = m_Renderer->getCharHeight(m_BitmapFont) * entry->scale;
		// quad buffer is centered by default so adjust position!
		uint32 idx = m_Buffer->create(dimX,dimY,0.0f,ds::Vec2(x + dimX * 0.5f ,y + dimY * 0.5f),cd.u1,cd.v1,cd.u2,cd.v2,entry->color);
		entry->vertices.push_back(idx);				
		x += dimX +4;
	}
}

}
