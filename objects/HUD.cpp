#include "HUD.h"
#include "..\utils\font.h"
#include "..\io\FileWatcher.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "..\utils\PlainTextReader.h"

namespace ds {

HUD::HUD() : GameObject() {}


HUD::~HUD() {
	LOG(logINFO) << "Destructing HUDEntity";	
}

void HUD::init(int textureID,const char* fontName) {
	m_Font = m_Renderer->loadBitmapFont(fontName,textureID);
	clear();
}

void HUD::clear() {
	for ( int i = 0; i < 128; ++i ) {
		m_HUDEntries[i].flag = -1;
		m_HUDEntries[i].sprites.clear();
	}
	for ( int i = 0 ; i < MAX_COUNTER; ++i ) {
		m_Counter[i].flag = -1;
	}
	for ( int i = 0; i < 32; ++i ) {
		m_TextEntries[i].flag = -1;
	}
	for ( int i = 0; i < MAX_TIMER; ++i ) {
		m_Timer[i].flag = -1;
	}
	for ( int i = 0; i < MAX_HUD_IMAGES; ++i ) {
		m_Images[i].flag = -1;
	}
}

// -------------------------------------------------------
// Add text
// -------------------------------------------------------
int HUD::addText(int x,int y,const std::string& txt,const ds::Color& color,float scale) {
	int id = -1;
	for ( int i = 0; i < 32 ; ++i ) {
		if ( m_TextEntries[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		HUDText* ht = &m_TextEntries[id];
		ht->entryID = createEntry(Vector2f(x,y),scale,color);		
		ht->text = txt;	
		ht->flag = 0;
		HUDEntry* he = &m_HUDEntries[ht->entryID];
		createText(he,txt,false);
	}
	return id;
}

// -------------------------------------------------------
// Add text
// -------------------------------------------------------
void HUD::addText(int id,int x,int y,const std::string& txt,const ds::Color& color,float scale) {
	int index = -1;
	for ( int i = 0; i < 32 ; ++i ) {
		if ( m_TextEntries[i].flag == -1 && index == -1 ) {
			index = i;
		}
	}
	if ( index != -1 ) {
		HUDText* ht = &m_TextEntries[index];
		ht->id = id;
		ht->entryID = createEntry(Vector2f(x,y),scale,color);		
		ht->text = txt;	
		ht->flag = 0;
		HUDEntry* he = &m_HUDEntries[ht->entryID];
		createText(he,txt,false);
	}
}

// -------------------------------------------------------
// Set text
// -------------------------------------------------------
void HUD::setText(int id,const std::string& txt) {
	int index = getTextIndex(id);
	assert(index >= 0 && index < 32);
	HUDText* ht = &m_TextEntries[index];
	ht->text = txt;	
	HUDEntry* he = &m_HUDEntries[ht->entryID];
	createText(he,txt,true);
}

// -------------------------------------------------------
// Add counter
// -------------------------------------------------------
void HUD::addCounter(int id,int x,int y,int length,int value,const ds::Color& color,float scale) {
	assert(id >= 0 && id < MAX_COUNTER);
	HUDCounter* hc = &m_Counter[id];
	assert(hc->flag == -1);
	hc->entryID = createEntry(Vector2f(x,y),scale,color);
	hc->id = id;
	hc->length = length;
	hc->value = -1;		
	hc->flag = 0;		
	setCounterValue(id,value);	
}

// -------------------------------------------------------
// Add image
// -------------------------------------------------------
void HUD::addImage(int id,int x,int y,const Rect& texturRect,const Color& color,float scale) {
	assert(id >= 0 && id < MAX_HUD_IMAGES);
	HUDImage* hi = &m_Images[id];
	assert(hi->flag == -1);
	hi->entryID = createEntry(Vector2f(x,y),scale,color);
	hi->id = id;
	HUDEntry* entry = &m_HUDEntries[hi->entryID];
	SpriteObject sp;
	sp.setPosition(Vector2f(x,y));
	sp.setTextureRect(texturRect);
	sp.setColor(color);
	sp.setScale(Vector2f(scale,scale));
	entry->sprites.push_back(sp);	
}

// -------------------------------------------------------
// Add timer
// -------------------------------------------------------
void HUD::addTimer(int id,int x,int y,const ds::Color& color,float scale) {
	assert(id >= 0 && id < MAX_TIMER);
	HUDTimer* hc = &m_Timer[id];
	assert(hc->flag == -1);
	hc->entryID = createEntry(Vector2f(x,y),scale,color);
	hc->id = id;			
	hc->flag = 0;	
	hc->gameTimer.reset();		
	hc->seconds = 0;
	setTimer(id,0,0);	
}

// -------------------------------------------------------
// Set timer
// -------------------------------------------------------
void HUD::setTimer(int id,int minutes,int seconds) {
	assert(id >= 0 && id < MAX_TIMER);
	std::string text;
	string::formatTime(minutes,seconds,text);
	HUDTimer* timer = &m_Timer[id];
	timer->gameTimer.set(0,seconds,minutes,0);
	HUDEntry* entry = &m_HUDEntries[timer->entryID];
	createText(entry,text,true);
}

// -------------------------------------------------------
// Get timer
// -------------------------------------------------------
GameTimer* HUD::getTimer(int id) {
	assert(id >= 0 && id < MAX_TIMER);
	HUDTimer* timer = &m_Timer[id];
	if ( timer != 0 ) {
		return &timer->gameTimer;
	}
	return 0;
}

// -------------------------------------------------------
// Start timer
// -------------------------------------------------------
void HUD::startTimer(int id) {
	assert(id >= 0 && id < MAX_TIMER);
	HUDTimer* timer = &m_Timer[id];
	timer->gameTimer.start();
}

// -------------------------------------------------------
// Stop timer
// -------------------------------------------------------
void HUD::stopTimer(int id) {
	assert(id >= 0 && id < MAX_TIMER);
	HUDTimer* timer = &m_Timer[id];
	timer->gameTimer.stop();
}

// -------------------------------------------------------
// Set counter value
// -------------------------------------------------------
void HUD::setCounterValue(int id,int value) {
	assert(id >= 0 && id < MAX_COUNTER);
	HUDCounter* hc = &m_Counter[id];
	if ( hc->value != value ) {
		hc->value = value;
		std::string text;
		string::formatInt(value,text,hc->length);
		HUDEntry* entry = &m_HUDEntries[hc->entryID];
		createText(entry,text,true);
	}
}

// -------------------------------------------------------
// Get index of counter by id
// -------------------------------------------------------
int HUD::getTextIndex(int id) {
	for ( int i = 0; i < 32; ++i ) {
		if ( m_TextEntries[i].id == id ) {
			return i;
		}
	}
	return -1;
}

// -------------------------------------------------------
// Draw internal buffer
// -------------------------------------------------------
void HUD::render() {
	
	for ( int i = 0; i < 128; ++i ) {
		if ( m_HUDEntries[i].flag != -1 ) {
			HUDEntry* entry = &m_HUDEntries[i];
			for ( size_t j = 0; j < entry->sprites.size(); ++j ) {
				m_Renderer->draw(entry->sprites[j]);
			}
		}
	}
	
}

// -------------------------------------------------------
// Tick all timer
// -------------------------------------------------------
void HUD::update(float elapsed) {
	for ( int i = 0; i < MAX_TIMER; ++i ) {
		if ( m_Timer[i].flag != -1 ) {
			HUDTimer* timer = &m_Timer[i];
			timer->gameTimer.tick(elapsed);
			if ( timer->seconds != timer->gameTimer.getSeconds()) {
				timer->seconds = timer->gameTimer.getSeconds();
				setTimer(timer->id,timer->gameTimer.getMinutes(),timer->gameTimer.getSeconds());			
			}
		}
	}
}
// -------------------------------------------------------
// Create text
// -------------------------------------------------------
void HUD::createText(HUDEntry* entry,const std::string& text,bool clear) {
	if ( clear ) {
		entry->sprites.clear();
	}
	font::createText(*m_Font,entry->pos,text,entry->color,entry->sprites,entry->scale,entry->scale);	
}

// -------------------------------------------------------
// Create new HUD entry
// -------------------------------------------------------
int HUD::createEntry(const Vector2f& pos,float scale,const ds::Color& color) {
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

void HUD::load(const char* name) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",name);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];			
			if ( c->getName() != "hud" ) {
				int id = c->getInt(0,"id");
				Vector2f pos = c->getVector2f("position");			
				Color clr = Color::WHITE;
				c->getColor("color",&clr);
				float scale = 1.0f;
				c->getFloat("scale",&scale);
				if ( c->getName() == "counter" ) {				
					int length = 6;
					c->getInt("length",&length);
					addCounter(id,pos.x,pos.y,length,0,clr,scale);
				}
				else if ( c->getName() == "timer" ) {
					addTimer(id,pos.x,pos.y,clr,scale);
				}
				else if ( c->getName() == "text" ) {
					addText(id,pos.x,pos.y,c->getProperty("text"),clr,scale);
				}
				else if ( c->getName() == "image" ) {
					Rect rect;
					c->getRect("rect",&rect);
					addImage(id,pos.x,pos.y,rect,clr,scale);
				}
			}
		}		
		gFileWatcher->registerFile(buffer,this);
	}
}

void HUD::reload(const char* fileName) {
	JSONReader reader;
	if ( reader.parse(fileName) ) {
		clear();
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];			
			if ( c->getName() != "hud" ) {
				int id = c->getInt(0,"id");
				Vector2f pos = c->getVector2f("position");			
				Color clr = Color::WHITE;
				c->getColor("color",&clr);
				float scale = 1.0f;
				c->getFloat("scale",&scale);
				if ( c->getName() == "counter" ) {				
					int length = 6;
					c->getInt("length",&length);
					addCounter(id,pos.x,pos.y,length,0,clr,scale);
				}
				else if ( c->getName() == "timer" ) {
					addTimer(id,pos.x,pos.y,clr,scale);
				}
				else if ( c->getName() == "text" ) {
					addText(id,pos.x,pos.y,c->getProperty("text"),clr,scale);
				}
				else if ( c->getName() == "image" ) {
					Rect rect;
					c->getRect("rect",&rect);
					addImage(id,pos.x,pos.y,rect,clr,scale);
				}
			}
		}		
	}
}

}
