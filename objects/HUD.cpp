#include "HUD.h"
#include "..\utils\font.h"
#include "..\io\FileWatcher.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "..\utils\PlainTextReader.h"
#include "..\compiler\HUDConverter.h"
#include "..\utils\Profiler.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"
#include "..\DialogResources.h"

namespace ds {

HUD::HUD() : _offset(0), _state(1), _showAdd(false) {
	_availableElements.push_back("Image");
	_availableElements.push_back("Text");
	_availableElements.push_back("Counter");
	_availableElements.push_back("Timer");
	_selectedElement = 0;
	_elementOffset = 0;
}


HUD::~HUD() {
	LOG << "Destructing HUDEntity";	
}

void HUD::init(int textureID,const char* fontName) {
	m_Font = renderer::loadBitmapFont(fontName,textureID);
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
	for (int i = 0; i < MAX_HUD_NUMBERS; ++i) {
		m_Numbers[i].flag = -1;
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
		LOGC("HUD") << "adding text - id: " << id;
		HUDText* ht = &m_TextEntries[id];
		ht->entryID = createEntry(HET_TEXT,v2(x,y),scale,color);		
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
	assert(id >= 0 && id < 32);
	LOGC("HUD") << "adding text - id: " << id;
	HUDText* ht = &m_TextEntries[id];
	assert(ht->flag == -1);
	ht->id = id;
	ht->entryID = createEntry(HET_TEXT, v2(x,y),scale,color);		
	ht->text = txt;	
	ht->flag = 0;
	HUDEntry* he = &m_HUDEntries[ht->entryID];
	createText(he,txt,false);
}

// -------------------------------------------------------
// remove text
// -------------------------------------------------------
void HUD::removeText(int id) {
	assert(id >= 0 && id < 32);
	LOGC("HUD") << "removing text - id: " << id;
	HUDText* hi = &m_TextEntries[id];
	hi->flag = -1;
	HUDEntry* entry = &m_HUDEntries[hi->entryID];
	entry->sprites.clear();
	entry->flag = -1;
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
	LOGC("HUD") << "adding counter - id: " << id;
	HUDCounter* hc = &m_Counter[id];
	assert(hc->flag == -1);
	hc->entryID = createEntry(HET_COUNTER, v2(x,y),scale,color);
	hc->id = id;
	hc->length = length;
	hc->value = -1;		
	hc->flag = 0;		
	setCounterValue(id,value);	
}

// -------------------------------------------------------
// remove counter
// -------------------------------------------------------
void HUD::removeCounter(int id) {
	assert(id >= 0 && id < MAX_HUD_IMAGES);
	LOGC("HUD") << "removing counter - id: " << id;
	HUDCounter* hi = &m_Counter[id];
	hi->flag = -1;
	HUDEntry* entry = &m_HUDEntries[hi->entryID];
	entry->sprites.clear();
	entry->flag = -1;
}

// -------------------------------------------------------
// Add image
// -------------------------------------------------------
void HUD::addImage(int id,int x,int y,const Rect& texturRect,const Color& color,float scale) {
	assert(id >= 0 && id < MAX_HUD_IMAGES);
	LOGC("HUD") << "adding image - id: " << id;
	HUDImage* hi = &m_Images[id];
	assert(hi->flag == -1);
	hi->entryID = createEntry(HET_IMAGE, v2(x,y),scale,color);
	hi->id = id;
	hi->flag = 1;
	HUDEntry* entry = &m_HUDEntries[hi->entryID];
	Sprite sp;
	sp.position = v2(0, 0);
	sp.texture = math::buildTexture(texturRect);
	sp.color = color;
	sp.scale = Vector2f(scale,scale);
	entry->sprites.push_back(sp);	
}

// -------------------------------------------------------
// remove image
// -------------------------------------------------------
void HUD::removeImage(int id) {
	assert(id >= 0 && id < MAX_HUD_IMAGES);
	LOGC("HUD") << "removing image - id: " << id;
	HUDImage* hi = &m_Images[id];
	hi->flag = -1;
	HUDEntry* entry = &m_HUDEntries[hi->entryID];
	entry->sprites.clear();
	entry->flag = -1;
}

// -------------------------------------------------------
// Add timer
// -------------------------------------------------------
void HUD::addTimer(int id,int x,int y,const ds::Color& color,float scale) {
	assert(id >= 0 && id < MAX_TIMER);
	LOGC("HUD") << "adding timer - id: " << id;
	HUDTimer* hc = &m_Timer[id];
	assert(hc->flag == -1);
	hc->entryID = createEntry(HET_TIMER, v2(x,y),scale,color);
	hc->id = id;			
	hc->flag = 0;	
	hc->gameTimer.reset();		
	hc->seconds = 0;
	setTimer(id,0,0);	
}

// -------------------------------------------------------
// remove timer
// -------------------------------------------------------
void HUD::removeTimer(int id) {
	assert(id >= 0 && id < MAX_HUD_IMAGES);
	LOGC("HUD") << "removing timer - id: " << id;
	HUDTimer* hi = &m_Timer[id];
	hi->flag = -1;
	HUDEntry* entry = &m_HUDEntries[hi->entryID];
	entry->sprites.clear();
	entry->flag = -1;
}

// -------------------------------------------------------
// add number
// -------------------------------------------------------
void HUD::addNumber(int id, int x, int y, int length, const ds::Color& color, float scale) {
	HUDNumber& n = m_Numbers[id];
	n.position = Vector2f(x, y);
	n.value = 0;
	n.num = length;
	n.color = color;
	n.flag = 1;
	setNumber(id, 0);
}

// -------------------------------------------------------
// set number
// -------------------------------------------------------
void HUD::setNumber(int id, int value) {
	HUDNumber& n = m_Numbers[id];
	int div = 1;
	int length = n.num;
	if (n.num == -1) {
		length = 0;
		int tmp = value / div;
		while (tmp >= 1) {
			div *= 10;
			tmp = value / div;
			++length;
		}
	}
	if (length >= 16) {
		length = 16;
	}
	if (length == 0) {
		length = 1;
	}
	div = (int)pow(10.0f, (length - 1.0f));
	Vector2f p = n.position;
	int tmp = value;
	float xp = 0.0f;
	for (int i = 0; i < length; ++i) {
		int r = tmp / div;
		if (r < 0 || r > 9) {
			r = 0;
		}
		n.digits[i] = r;
		n.textures[i] = m_Definitions[r].texture;
		n.positions[i] = p;
		p.x += m_Definitions[r].width;
		xp += m_Definitions[r].width;
		tmp = tmp - r * div;
		div /= 10;
	}
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
void HUD::setCounterValue(int id,int value,bool force) {
	assert(id >= 0 && id < MAX_COUNTER);
	HUDCounter* hc = &m_Counter[id];
	if ( hc->value != value || force ) {
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
	PRS("HUD::draw")
	for ( int i = 0; i < 128; ++i ) {
		if ( m_HUDEntries[i].flag != -1 ) {
			HUDEntry* entry = &m_HUDEntries[i];
			for ( size_t j = 0; j < entry->sprites.size(); ++j ) {
				v2 p = entry->sprites[j].position;
				p += entry->pos;
				sprites::draw(p, entry->sprites[j].texture, 0.0f, entry->sprites[j].scale.x, entry->sprites[j].scale.y, entry->color);
			}
		}
	}
	for (int i = 0; i < MAX_HUD_NUMBERS; ++i) {
		if (m_Numbers[i].flag != -1) {
			HUDNumber& n = m_Numbers[i];
			for (int j = 0; j < n.num; ++j) {
				sprites::draw(n.positions[j], n.textures[j]);
			}
		}
	}
	PRE("HUD::draw")
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
	font::createText(*m_Font, v2(0,0), text.c_str(), entry->color, entry->sprites, entry->scale, entry->scale);
}

// -------------------------------------------------------
// define number
// -------------------------------------------------------
void HUD::defineNumber(int index, float top, float left, float width, float height) {
	assert(index >= 0 && index < 10);
	NumberDef& def = m_Definitions[index];
	def.start = left;
	def.width = width;
	def.texture = math::buildTexture(top, left, width, height);
}
// -------------------------------------------------------
// Create new HUD entry
// -------------------------------------------------------
int HUD::createEntry(HudEntryType type, const Vector2f& pos, float scale, const ds::Color& color) {
	int id = -1;
	for ( int i = 0; i < 128; ++i ) {
		if ( m_HUDEntries[i].flag == -1 && id == -1 ) {
			id = i;
		}
	}
	if ( id != -1 ) {
		m_HUDEntries[id].type = type;
		m_HUDEntries[id].color = color;
		m_HUDEntries[id].pos = pos;
		m_HUDEntries[id].flag = 0;
		m_HUDEntries[id].scale = scale;
	}
	return id;
}

// -------------------------------------------------------
// remove
// -------------------------------------------------------
void HUD::remove(uint32 id, HudEntryType type) {
	switch (type) {
		case HET_IMAGE: removeImage(id); break;
		case HET_TIMER: removeTimer(id); break;
		case HET_COUNTER: removeCounter(id); break;
		case HET_TEXT: removeText(id); break;
	}
}

// -------------------------------------------------------
// find next available ID by type
// -------------------------------------------------------
int HUD::findFreeIndex(HudEntryType type) {
	if (type == HET_IMAGE) {
		for (int i = 0; i < MAX_HUD_IMAGES; ++i) {
			if (m_Images[i].flag == -1) {
				return i;
			}
		}
	}
	else if (type == HET_TEXT) {
		for (int i = 0; i < 32; ++i) {
			if (m_TextEntries[i].flag == -1) {
				return i;
			}
		}
	}
	else if (type == HET_COUNTER) {
		for (int i = 0; i < MAX_COUNTER; ++i) {
			if (m_Counter[i].flag == -1) {
				return i;
			}
		}
	}
	else if (type == HET_TIMER) {
		for (int i = 0; i < MAX_TIMER; ++i) {
			if (m_Timer[i].flag == -1) {
				return i;
			}
		}
	}
	return -1;
}

// -------------------------------------------------------
// load binary file
// -------------------------------------------------------
void HUD::load(BinaryLoader* loader) {	
	clear();
	_model.clear();
	char buffer[32];
	while ( loader->openChunk() == 0 ) {
		if (loader->getChunkID() == 5) {
			Rect r;
			for (int i = 0; i < 10; ++i) {
				loader->read(&r);
				defineNumber(i, r.top, r.left, r.width(), r.height());
			}
		}
		else {
			int id = 0;
			loader->read(&id);
			Vector2f pos;
			loader->read(&pos);
			Color clr(255,255,255,255);
			loader->read(&clr);
			float scale = 1.0f;
			loader->read(&scale);
			if ( loader->getChunkID() == 1 ) {							
				int length = 6;
				loader->read(&length);
				addCounter(id,pos.x,pos.y,length,0,clr,scale);
				addToModel(id, HET_COUNTER, "Counter");
			}
			else if ( loader->getChunkID() == 2 ) {							
				addTimer(id,pos.x,pos.y,clr,scale);
				addToModel(id, HET_TIMER, "Timer");
			}
			else if ( loader->getChunkID() == 3 ) {							
				std::string str;
				loader->read(str);
				addText(id,pos.x,pos.y,str,clr,scale);
				addToModel(id, HET_TEXT, "Text");				
			}
			else if ( loader->getChunkID() == 4 ) {	
				Rect rect;
				loader->read(&rect);
				addImage(id,pos.x,pos.y,rect,clr,scale);
				addToModel(id, HET_IMAGE, "Image");
			}		
			else if (loader->getChunkID() == 6) {
				int length = -1;
				loader->read(&length);
				addNumber(id, pos.x, pos.y, length, clr, scale);
				addToModel(id, HET_NUMBER, "Number");
			}
		}
		loader->closeChunk();
	}		
}

// -------------------------------------------------------
// add model entry
// -------------------------------------------------------
void HUD::addToModel(int id, HudEntryType type, const char* prefix) {
	char buffer[32];
	sprintf_s(buffer, 32, "%s %d",prefix, id);
	HudGUIElement hge;
	hge.id = id;
	hge.type = type;
	_model.add(buffer, hge);
}

// -------------------------------------------------------
// show add new item dialog
// -------------------------------------------------------
void HUD::showAddDialog() {
	if (_showAdd) {
		if (gui::begin("Add element", &_state)) {
			gui::ComboBox(1, _availableElements, &_selectedElement, &_elementOffset);
			gui::beginGroup();
			if (gui::Button(21, "OK")) {
				if (_selectedElement != -1) {
					if (_selectedElement == 0) {
						int id = findFreeIndex(HET_IMAGE);
						if (id != -1) {
							addImage(id, 512, 384, ds::Rect(0, 0, 50, 50));
							addToModel(id, HET_IMAGE, "Image");
						}
					}
					else if (_selectedElement == 1) {
						int id = findFreeIndex(HET_TEXT);
						if (id != -1) {
							addText(id, 512, 384, "Test");
							addToModel(id, HET_TEXT, "Text");
						}
					}
					else if (_selectedElement == 2) {
						int id = findFreeIndex(HET_COUNTER);
						if (id != -1) {
							addCounter(id, 512, 384, 1, 1);
							addToModel(id, HET_COUNTER, "Counter");
						}
					}
					else if (_selectedElement == 3) {
						int id = findFreeIndex(HET_TIMER);
						if (id != -1) {
							addTimer(id, 512, 384);
							addToModel(id, HET_TIMER, "Timer");
						}
					}
				}
				_showAdd = false;
			}
			if (gui::Button(22, "Cancel")) {
				_showAdd = false;
			}
			gui::endGroup();
		}
		gui::end();
	}
}
// -------------------------------------------------------
// show dialog
// -------------------------------------------------------
void HUD::showDialog(v2* pos) {
	gui::start(HUD_ID, pos);
	if (gui::begin("HUD", &_state)) {
		gui::ComboBox(1, &_model, &_offset);
		gui::beginGroup();
		if (gui::Button(2, "Save")) {
			LOG << "Save pressed";
		}
		if (gui::Button(20, "Add")) {
			_showAdd = !_showAdd;
		}
		gui::endGroup();
	}
	gui::end();
	
	showAddDialog();

	if (_model.hasSelection()) {
		if (gui::begin("HUD Element", &_state)) {		
			HudGUIElement element = _model.getSelectedValue();
			if ( element.type == HET_COUNTER) {
				HUDCounter& counter = m_Counter[element.id];
				HUDEntry& entry = m_HUDEntries[counter.entryID];
				gui::InputVec2(3, "Position", &entry.pos);
				gui::InputFloat(4, "Scale", &entry.scale);
				gui::InputColor(5, "Color", &entry.color);
				gui::InputInt(6, "Length", &counter.length);
				setCounterValue(counter.id, 0, true);
			}
			else if (element.type == HET_IMAGE) {
				HUDImage& image = m_Images[element.id];
				HUDEntry& entry = m_HUDEntries[image.entryID];
				Sprite& sprite = entry.sprites[0];
				gui::InputVec2(3, "Position", &entry.pos);
				gui::InputFloat(4, "Scale", &entry.scale);
				gui::InputColor(5, "Color", &entry.color);
				ds::Rect r = sprite.texture.rect;
				gui::InputRect(6, "TextureRect", &r);
				sprite.texture = math::buildTexture(r);
			}
			else if (element.type == HET_TIMER) {
				HUDTimer& timer = m_Timer[element.id];
				HUDEntry& entry = m_HUDEntries[timer.entryID];
				Sprite& sprite = entry.sprites[0];
				gui::InputVec2(3, "Position", &entry.pos);
				gui::InputFloat(4, "Scale", &entry.scale);
				gui::InputColor(5, "Color", &entry.color);
			}
			else if (element.type == HET_TEXT) {
				HUDText& txt = m_TextEntries[element.id];
				HUDEntry& entry = m_HUDEntries[txt.entryID];
				Sprite& sprite = entry.sprites[0];
				gui::InputVec2(3, "Position", &entry.pos);
				gui::InputFloat(4, "Scale", &entry.scale);
				gui::InputColor(5, "Color", &entry.color);
			}
			if (gui::Button(23, "Remove")) {
				LOG << "Remove pressed";
				remove(element.id, element.type);
				_model.remove(_model.getSelection());
				_model.select(-1);
			}
		}
	}
	gui::end();	
}


}
