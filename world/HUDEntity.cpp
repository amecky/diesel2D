#include "HUDEntity.h"
#include "..\utils\font.h"

namespace ds {

HUDEntity::HUDEntity() : Entity() , m_Buffer(0) {
	
}


HUDEntity::~HUDEntity() {
	LOG(logINFO) << "Destructing HUDEntity";
	if ( m_Buffer != 0 ) {
		delete m_Buffer;
	}
}

void HUDEntity::init(ds::Renderer* renderer,int textureID,const char* fontName) {
	font::load(fontName,renderer,textureID,m_Font);
	m_Buffer = new SpriteBatch(renderer,1024,textureID);
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
// Add text
// -------------------------------------------------------
int HUDEntity::addText(int x,int y,const std::string& txt,const ds::Color& color,float scale) {
	int id = -1;
	for ( int i = 0; i < 32 ; ++i ) {
		if ( m_TextEntries[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		HUDText* ht = &m_TextEntries[id];
		ht->entryID = createEntry(Vec2(x,y),scale,color);		
		ht->text = txt;	
		ht->flag = 0;
		HUDEntry* he = &m_HUDEntries[ht->entryID];
		createText(he,txt,false);
	}
	return id;
}

// -------------------------------------------------------
// Add counter
// -------------------------------------------------------
int HUDEntity::addCounter(int x,int y,int length,int value,const ds::Color& color,float scale) {
	int id = -1;
	for ( int i = 0; i < 16; ++i ) {
		if ( m_Counter[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		HUDCounter* hc = &m_Counter[id];
		hc->entryID = createEntry(Vec2(x,y),scale,color);
		hc->id = id;
		hc->length = length;
		hc->value = value;		
		hc->flag = 0;		
		setCounterValue(id,value);
	}	
	return id;
}

// -------------------------------------------------------
// Set counter value
// -------------------------------------------------------
void HUDEntity::setCounterValue(int id,int value) {
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
void HUDEntity::draw() {
	m_Buffer->begin();
	for ( int i = 0; i < 128; ++i ) {
		if ( m_HUDEntries[i].flag != -1 ) {
			HUDEntry* entry = &m_HUDEntries[i];
			for ( size_t j = 0; j < entry->sprites.size(); ++j ) {
				m_Buffer->draw(entry->sprites[j]);
			}
		}
	}
	m_Buffer->end();
}

// -------------------------------------------------------
// Tick all timer
// -------------------------------------------------------
void HUDEntity::update(float elapsed) {
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
				//setTimer(timer->id,timer->minutes,timer->seconds);
			}
		}
	}
}
// -------------------------------------------------------
// Create text
// -------------------------------------------------------
void HUDEntity::createText(HUDEntry* entry,const std::string& text,bool clear) {
	if ( clear ) {
		entry->sprites.clear();
	}
	font::createText(m_Font,entry->pos,text,entry->color,entry->sprites,entry->scale,entry->scale);	
}

// -------------------------------------------------------
// Create new HUD entry
// -------------------------------------------------------
int HUDEntity::createEntry(const ds::Vec2& pos,float scale,const ds::Color& color) {
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

}
