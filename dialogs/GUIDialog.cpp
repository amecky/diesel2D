#include "GUIDialog.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"
#include "..\DialogResources.h"

namespace ds {
	GUIDialog::GUIDialog() {
		_state = 1;
		_offset = 0;
		_position = v2(1050, 690);
		_availableElements.push_back("Image");
		_availableElements.push_back("Text");
		_availableElements.push_back("Button");
		_availableElements.push_back("ImageLink");
		_showAdd = false;
		_selectedElement = 0;
		_elementOffset = 0;
	}
	// -------------------------------------------------------
	// Init
	// -------------------------------------------------------
	void GUIDialog::init(const char* name,const DialogID& id,BitmapFont* bitmapFont) {	
		m_ID = id;
		m_BitmapFont = bitmapFont;
		m_Active = false;
		m_HashName = string::murmur_hash(name);
		m_SupportHover = false;
		m_SelectedInput = -1;
		load();
	}


	// -------------------------------------------------------
	// Destructor
	// -------------------------------------------------------
	GUIDialog::~GUIDialog(void) {
		Effects::iterator it = m_Effects.begin();
		while (it != m_Effects.end()) {
			delete (*it);
			it = m_Effects.erase(it);
		}
	}

	// -------------------------------------------------------
	// Add static image
	// -------------------------------------------------------
	uint32 GUIDialog::addImage(int id,int x,int y,const Rect& textureRect,bool centered) {
		assert(!containsItem(id));
		Vector2f p = Vector2f(x,y);
		if ( centered ) {
			p.x = renderer::getScreenWidth() * 0.5f;
		}
		GUIItem item;
		item.type = GIT_IMAGE;
		item.pos = p;
		item.id = id;
		item.texture = math::buildTexture(textureRect);
		item.centered = centered;
		item.color = Color::WHITE;
		item.scale = 1.0f;
		m_Items.push_back(item);
		return item.id;
	}

	// -------------------------------------------------------
	// update image
	// -------------------------------------------------------
	void GUIDialog::updateImage(int id, int x, int y, const Rect& textureRect, bool centered) {
		GUIItem* item = findByID(id);
		assert(item != 0);
		item->pos = v2(x,y);
		item->texture = math::buildTexture(textureRect);
		item->centered = centered;
	}

	// -------------------------------------------------------
	// Add image link
	// -------------------------------------------------------
	uint32 GUIDialog::addImageLink(int id,int x,int y,const Rect& textureRect,bool centered) {
		assert(!containsItem(id));
		Vector2f p = Vector2f(x, y);
		if (centered) {
			p.x = renderer::getScreenWidth() * 0.5f;
		}
		GUIItem item;
		item.type = GIT_IMAGELINK;
		item.pos = p;
		item.id = id;
		float w = textureRect.width();
		float h = textureRect.height();
		item.boundingRect = Rect(h * 0.5f, w * -0.5f, w, -h);
		m_Items.push_back(item);
		return item.id;
	}

	// -------------------------------------------------------
	// Adds a text
	// -------------------------------------------------------
	void GUIDialog::addText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
		assert(!containsItem(id));
		Vector2f p = Vector2f(x,y);
		GUIItem item;
		item.pos = p;
		item.type = GIT_TEXT;
		strcpy(item.label, text.c_str());
		item.textSize = text.size();
		item.id = id;
		item.centered = centered;
		item.color = color;
		item.scale = scale;
		//item.size = size;
		m_Items.push_back(item);
	}

	// -------------------------------------------------------
	// Update text
	// -------------------------------------------------------
	void GUIDialog::updateText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
		GUIItem* item = findByID(id);
		assert(item != 0);
		item->centered = centered;
		item->color = color;
		item->scale = scale;
		item->pos = v2(x,y);
		//item->size = size;
		strcpy(item->label, text.c_str());
		item->textSize = text.size();
	}

	// -------------------------------------------------------
	// Update text
	// -------------------------------------------------------
	void GUIDialog::updateText(int id,const std::string& text) {	
		GUIItem* item = findByID(id);
		assert(item != 0);
		strcpy(item->label, text.c_str());
		item->textSize = text.size();
	}

	v2 GUIDialog::getTextSize(int id) {
		GUIItem* item = findByID(id);
		assert(item != 0);
		return item->size;
	}

	// -------------------------------------------------------
	// On button
	// -------------------------------------------------------
	int GUIDialog::onButton(int button,int x,int y,bool down) {
		if ( !m_Active ) {
			return -1;
		}
		for (size_t i = 0; i < m_Items.size(); ++i) {
			if (m_Items[i].type == GIT_BUTTON || m_Items[i].type == GIT_IMAGELINK) {
				Rect br = m_Items[i].boundingRect;
				br.left += m_Items[i].pos.x;
				br.right += m_Items[i].pos.x;
				br.top += m_Items[i].pos.y;
				br.bottom += m_Items[i].pos.y;
				if ( x >= br.left && x <= br.right && y <= br.top && y >= br.bottom ) {
					return m_Items[i].id;
				}		
			}
		}	
		return -1;
	}
	
	// -------------------------------------------------------
	// Set button texture
	// -------------------------------------------------------
	void GUIDialog::setButtonTexture(int id,const Rect& textureRect) {
		/*
		for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
			DialogButton* button = &m_Buttons[i];
			if ( button->id == id ) {		
				GUIItem* item = &m_Items[button->imageIndex];
				for ( size_t j = 0; j < item->sprites.size(); ++j ) {
					item->sprites[j].texture = math::buildTexture(textureRect);
				}
			}
		}
		*/
		for (size_t i = 0; i < m_Items.size(); ++i) {
			GUIItem& item = m_Items[i];
			if (item.type == GIT_BUTTON && item.id == id) {
				item.texture = math::buildTexture(textureRect);
			}
		}
	}

	// -------------------------------------------------------
	// add button 
	// -------------------------------------------------------
	void GUIDialog::addButton(int id,float x,float y, const char* text, const Rect& textureRect, const Color& textColor, float textScale, bool centered) {
		assert(!containsItem(id));
		if (centered) {
			x = renderer::getScreenWidth() * 0.5f;
		}
		Vector2f p = Vector2f(x, y);
		GUIItem item;
		item.pos = p;
		item.type = GIT_BUTTON;
		item.texture = math::buildTexture(textureRect);
		item.scale = 1.0f;
		item.centered = centered;
		strcpy(item.label, text);
		item.textSize = strlen(text);
		item.id = id;
		item.color = Color::WHITE;
		float w = textureRect.width();
		float h = textureRect.height();
		item.boundingRect = Rect(h * 0.5f, w * -0.5f, w, -h);
		m_Items.push_back(item);
	}

	// -------------------------------------------------------
	// Activate
	// -------------------------------------------------------
	void GUIDialog::activate() {
		m_Active = true;
		/*
		if ( m_SupportHover ) {
			for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
				DialogButton* db = &m_Buttons[i];		
				setButtonTexture(db->id,m_ButtonItem);
			}
			m_Selected = -1;
		}
		*/
		m_SelectedInput = -1;	
		for (size_t i = 0; i < m_Effects.size(); ++i) {
			m_Effects[i]->setActive(true);
		}
	}

	// -------------------------------------------------------
	// Deactivate
	// -------------------------------------------------------
	void GUIDialog::deactivate() {
		m_Active = false;
	}
	// -------------------------------------------------------
	// Render both nodes
	// -------------------------------------------------------
	void GUIDialog::render() {
		if ( m_Active ) {
			for ( size_t i = 0; i < m_Items.size(); ++i ) {
				GUIItem* gi = &m_Items[i];
				if (gi->type == GIT_IMAGE) {
					v2 p = gi->pos;
					if (gi->centered) {
						p.x = renderer::getScreenWidth() * 0.5f;
					}
					sprites::draw(p, gi->texture, 0.0f, gi->scale, gi->scale, gi->color);
				}
				else if (gi->type == GIT_BUTTON) {
					v2 p = gi->pos;
					if (gi->centered) {
						p.x = renderer::getScreenWidth() * 0.5f;
					}
					sprites::draw(p, gi->texture, 0.0f, gi->scale, gi->scale, gi->color);					
					v2 size = font::calculateSize(*m_BitmapFont, gi->label, gi->scale);
					float ty = p.y - size.y * 0.5f;
					p += v2(size.x * -0.5f, -size.y * 0.5f);
					ds::sprites::drawText(m_BitmapFont, p.x, p.y, gi->label, 2.0f);
				}
				else if (gi->type == GIT_TEXT) {
					v2 p = gi->pos;
					if (gi->centered) {
						p.x = renderer::getScreenWidth() * 0.5f;
					}
					v2 size = font::calculateSize(*m_BitmapFont, gi->label, 4, gi->scale, gi->scale);
					float ty = p.y - size.y * 0.5f;
					p += v2(size.x * -0.5f, -size.y * 0.5f);
					ds::sprites::drawText(m_BitmapFont, p.x, p.y, gi->label, 4,gi->scale,gi->scale,gi->color);
				}
			}
		}
	}

	// -------------------------------------------------------
	// Update Mouse pos and set button textures if enabled
	// -------------------------------------------------------
	void GUIDialog::updateMousePos(const Vector2f& mousePos) {
		if ( m_SupportHover ) {
			int ret = onButton(0,mousePos.x,mousePos.y,false);	
			for ( size_t i = 0; i < m_Items.size(); ++i ) {
				//DialogButton* db = &m_Buttons[i];		
				//if ( db->id == ret && ret != m_Selected ) {
					//setButtonTexture(db->id,m_ButtonItemSelected);
				//}
			}
			if ( ret == -1 && m_Selected != -1 ) {
				//for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
					//DialogButton* db = &m_Buttons[i];		
					//if ( m_Selected == db->id ) {
						//setButtonTexture(db->id,m_ButtonItem);
					//}
				//}
			}
			m_Selected = ret;
		}
	}

	// -------------------------------------------------------
	// tick
	// -------------------------------------------------------
	void GUIDialog::tick(float dt) {
		for (size_t i = 0; i < m_Effects.size(); ++i) {
			m_Effects[i]->tick(dt);
		}
	}

	// -------------------------------------------------------
	// Set button hover
	// -------------------------------------------------------
	void GUIDialog::setButtonHover(const Rect& regularItem,const Rect& highlightItem) {
		m_SupportHover = true;
		m_ButtonItem = regularItem;
		m_ButtonItemSelected = highlightItem;
	}

	// -------------------------------------------------------
	// Find by id
	// -------------------------------------------------------
	GUIItem* GUIDialog::findByID(int id) {
		for ( size_t i = 0; i < m_Items.size(); ++i ) {
			GUIItem* gi = &m_Items[i];
			if ( gi->id == id ) {
				return gi;
			}
		}
		return 0;
	}

	// -------------------------------------------------------
	// Find by id
	// -------------------------------------------------------
	int GUIDialog::getNextID() {
		int id = 0;
		for (size_t i = 0; i < m_Items.size(); ++i) {
			GUIItem* gi = &m_Items[i];
			if (gi->id > id ) {
				id = gi->id;
			}
		}
		++id;
		return id;
	}

	// -------------------------------------------------------
	// contains item
	// -------------------------------------------------------
	bool GUIDialog::containsItem(int id) {
		for (size_t i = 0; i < m_Items.size(); ++i) {
			if (m_Items[i].id == id) {
				return true;
			}
		}
		return false;
	}
	// -------------------------------------------------------
	// Find by id
	// -------------------------------------------------------
	int GUIDialog::findFreeID() {
		for ( size_t i = 0; i < m_Items.size(); ++i ) {
			GUIItem* gi = &m_Items[i];
			if ( gi->id != i ) {
				return i;
			}
		}
		return m_Items.size();
	}

	bool GUIDialog::swap(int currentIndex, int newIndex) {
		if (currentIndex >= 0 && currentIndex < m_Items.size() && newIndex >= 0 && newIndex < m_Items.size()) {
			GUIItem current = m_Items[currentIndex];
			GUIItem next = m_Items[newIndex];
			m_Items[newIndex] = current;
			m_Items[currentIndex] = next;
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// old load method
	// -------------------------------------------------------
	void GUIDialog::load(BinaryLoader* loader) {
		clear();
		while ( loader->openChunk() == 0 ) {	
			if ( loader->getChunkID() == CHNK_DLG_IMAGE ) {
				int id = 0;
				loader->read(&id);
				Rect r;
				loader->read(&r);
				Vector2f p;
				loader->read(&p);
				int cnt = 0;
				loader->read(&cnt);
				bool centered = false;
				if ( cnt == 1 ) {
					centered = true;
				}
				addImage(id,p.x,p.y,r,centered);
				addToModel(id, GIT_IMAGE, "Image");
			}
			if (loader->getChunkID() == CHNK_DLG_IMAGE_LINK) {
				int id = 0;
				loader->read(&id);
				Rect r;
				loader->read(&r);
				Vector2f p;
				loader->read(&p);
				int cnt = 0;
				loader->read(&cnt);
				bool centered = false;
				if (cnt == 1) {
					centered = true;
				}
				addImageLink(id, p.x, p.y, r, centered);
				addToModel(id, GIT_IMAGELINK, "Link");
			}
			else if ( loader->getChunkID() == CHNK_DLG_BUTTON ) {
				int id = 0;
				loader->read(&id);
				Rect r;
				loader->read(&r);
				Vector2f p;
				loader->read(&p);
				std::string txt;
				loader->read(txt);			
				Color c;
				loader->read(&c);
				int cnt = 0;
				loader->read(&cnt);
				bool centered = false;
				if (cnt == 1) {
					centered = true;
				}
				addButton(id,p.x,p.y,txt.c_str(),r,c,1.0f,centered);
				addToModel(id, GIT_BUTTON, "Button");
			}
			else if ( loader->getChunkID() == CHNK_DLG_TEXT ) {
				int id = 0;
				loader->read(&id);
				Vector2f pos;
				loader->read(&pos);
				float scale = 1.0f;
				loader->read(&scale);
				std::string str;
				loader->read(str);
				Color clr = Color::WHITE;
				loader->read(&clr);
				int cnt = 0;
				bool centered = false;
				loader->read(&cnt);
				if ( cnt == 1 ) {
					centered = true;
				}			
				addText(id,pos.x,pos.y,str,clr,scale,centered);
				addToModel(id, GIT_TEXT, "Text");
			}		
			loader->closeChunk();
		}	
	}

	// -------------------------------------------------------
	// show add new item dialog
	// -------------------------------------------------------
	void GUIDialog::showAddDialog() {
		if (_showAdd) {
			if (gui::begin("Add element", &_state)) {
				gui::ComboBox(GUI_DIALOG_ID + 24, _availableElements, &_selectedElement, &_elementOffset);
				gui::beginGroup();
				if (gui::Button(GUI_DIALOG_ID + 25, "OK")) {
					if (_selectedElement != -1) {
						int id = getNextID();
						if (_selectedElement == 0) {
							addImage(id, 512, 384, Rect(0,0,50,50), true);
							addToModel(id, GIT_IMAGE, "Image");
						}
						else if (_selectedElement == 1) {
							addText(id, 512, 384, "Text", Color::WHITE, 1.0f, true);
							addToModel(id, GIT_TEXT, "Text");
						}
						else if (_selectedElement == 2) {
							addButton(id, 512, 384, "Text", Rect(0,0,50,50), Color::WHITE, 1.0f, true);
							addToModel(id, GIT_BUTTON, "Button");
						}
					}
					_showAdd = false;
				}
				if (gui::Button(GUI_DIALOG_ID + 26, "Cancel")) {
					_showAdd = false;
				}
				gui::endGroup();
			}
			gui::end();
		}
	}

	// -------------------------------------------------------
	// add to model
	// -------------------------------------------------------
	void GUIDialog::addToModel(int id, GUIItemType type,const char* prefix) {
		char buffer[32];
		sprintf_s(buffer, 32, "%s %d", prefix, id);
		GUIModelItem hge;
		hge.id = id;
		hge.type = type;
		_model.add(buffer, hge);
	}

	// -------------------------------------------------------
	// show dialog
	// -------------------------------------------------------
	void GUIDialog::showDialog() {
		if (gui::begin("GUI Dialog", &_state)) {
			gui::ComboBox(GUI_DIALOG_ID + 1, &_model, &_offset);
			gui::beginGroup();
			if (gui::Button(GUI_DIALOG_ID + 2, "Save")) {
				save();
			}
			if (gui::Button(GUI_DIALOG_ID + 11, "Load")) {
				load();
			}
			if (gui::Button(GUI_DIALOG_ID + 10, "Add")) {
				_showAdd = !_showAdd;
			}
			if (gui::Button(GUI_DIALOG_ID + 12, "Up")) {
				if (_model.hasSelection()) {
					int index = _model.getSelection();
					if (swap(index, index - 1)) {
						_model.swap(index, index - 1);
						_model.select(index - 1);
					}
				}
			}
			if (gui::Button(GUI_DIALOG_ID + 13, "Down")) {
				if (_model.hasSelection()) {
					int index = _model.getSelection();
					if (swap(index, index + 1)) {
						_model.swap(index, index + 1);
						_model.select(index + 1);
					}
				}
			}
			gui::endGroup();
		}
		gui::end();

		showAddDialog();

		if (_model.hasSelection()) {
			if (gui::begin("GUI Element", &_state)) {
				GUIModelItem element = _model.getSelectedValue();
				GUIItem* item = findByID(element.id);
				char buffer[32];
				sprintf_s(buffer, 32, "ID: %d", item->id);
				gui::Label(GUI_DIALOG_ID + 8, buffer);
				gui::InputVec2(GUI_DIALOG_ID + 3, "Position", &item->pos);
				gui::InputFloat(GUI_DIALOG_ID + 4, "Scale", &item->scale);
				gui::InputColor(GUI_DIALOG_ID + 5, "Color", &item->color);
				gui::CheckBox(GUI_DIALOG_ID + 6, "Centered", &item->centered);				
				if (element.type == GIT_IMAGE) {
					Rect r = item->texture.rect;
					gui::InputRect(GUI_DIALOG_ID + 8, "Texture", &r);
					item->texture = math::buildTexture(r);
				}
				else if (element.type == GIT_BUTTON) {
					Rect r = item->texture.rect;
					gui::InputRect(GUI_DIALOG_ID + 8, "Texture", &r);
					item->texture = math::buildTexture(r);
					gui::Input(GUI_DIALOG_ID + 7, "Label", item->label, 32);
				}
				else if (element.type == GIT_TEXT) {
					GUIItem* item = findByID(element.id);
					gui::Input(GUI_DIALOG_ID + 7, "Label", item->label,32);
				}
			}
			gui::end();
		}
	}

	// -------------------------------------------------------
	// save
	// -------------------------------------------------------
	void GUIDialog::save() {
		char buffer[64];
		sprintf(buffer, "assets\\%u", m_HashName);
		BinaryWriter writer;
		int signature[] = { 0, 8, 15 };
		writer.open(buffer, signature, 3);
		for (size_t i = 0; i < m_Items.size(); ++i) {
			GUIItem* gi = &m_Items[i];
			if (gi->type == GIT_IMAGE) {
				writer.startChunk(CHNK_DLG_IMAGE, 1);
				writer.write(gi->id);
				writer.write(gi->texture.rect);
				writer.write(gi->pos);
				int cnt = 0;
				if (gi->centered) {
					cnt = 1;
				}				
				writer.write(cnt);
				writer.write(gi->scale);
				writer.closeChunk();
			}			
			else if (gi->type == GIT_BUTTON) {
				writer.startChunk(CHNK_DLG_BUTTON, 1);
				writer.write(gi->id);
				writer.write(gi->texture.rect);
				writer.write(gi->pos);
				writer.write(gi->label);
				writer.write(gi->color);
				int cnt = 0;
				if (gi->centered) {
					cnt = 1;
				}
				writer.write(cnt);
				writer.write(gi->scale);
				writer.closeChunk();
			}
			else if (gi->type == GIT_TEXT) {
				writer.startChunk(CHNK_DLG_TEXT, 1);
				writer.write(gi->id);
				writer.write(gi->pos);
				writer.write(gi->label);
				writer.write(gi->color);
				int cnt = 0;
				if (gi->centered) {
					cnt = 1;
				}
				writer.write(cnt);
				writer.write(gi->scale);
				writer.closeChunk();
			}			
		}
		writer.close();
	}

	// -------------------------------------------------------
	// load
	// -------------------------------------------------------
	void GUIDialog::load() {
		clear();
		_model.clear();
		BinaryLoader loader;
		char buffer[64];
		sprintf(buffer, "assets\\%u", m_HashName);
		int signature[] = { 0, 8, 15 };
		loader.open(buffer, signature, 3);
		while (loader.openChunk() == 0) {
			if (loader.getChunkID() == CHNK_DLG_IMAGE) {
				int id = 0;
				loader.read(&id);
				Rect r;
				loader.read(&r);
				Vector2f p;
				loader.read(&p);
				int cnt = 0;
				loader.read(&cnt);
				bool centered = false;
				if (cnt == 1) {
					centered = true;
				}
				float s = 1.0f;
				loader.read(&s);
				addImage(id, p.x, p.y, r, centered);
				addToModel(id, GIT_IMAGE, "Image");
			}
			else if (loader.getChunkID() == CHNK_DLG_BUTTON) {
				char text[32];
				int id = 0;
				loader.read(&id);
				Rect r;
				loader.read(&r);
				Vector2f p;
				loader.read(&p);
				loader.read(text);				
				Color clr = Color::WHITE;
				loader.read(&clr);
				int cnt = 0;
				loader.read(&cnt);
				bool centered = false;
				if (cnt == 1) {
					centered = true;
				}
				float s = 1.0f;
				loader.read(&s);
				addButton(id, p.x, p.y, text, r, clr, s, centered);
				addToModel(id, GIT_TEXT, "Button");
			}
			else if (loader.getChunkID() == CHNK_DLG_TEXT) {
				char text[32];
				int id = 0;
				loader.read(&id);
				Vector2f p;
				loader.read(&p);
				loader.read(text);
				Color clr = Color::WHITE;
				loader.read(&clr);
				int cnt = 0;
				loader.read(&cnt);
				bool centered = false;
				if (cnt == 1) {
					centered = true;
				}
				float s = 1.0f;
				loader.read(&s);
				addText(id, p.x, p.y, text, clr, s, centered);
				addToModel(id, GIT_TEXT, "Text");
			}
			loader.closeChunk();
		}
	}
}
