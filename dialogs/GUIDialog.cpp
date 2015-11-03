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
		_availableElements.push_back("Number");
		_availableElements.push_back("Timer");
		_showAdd = false;
		_selectedElement = 0;
		_elementOffset = 0;
		_idIndex = 0;
		for (int i = 0; i < 32; ++i) {
			_ids[i].id = -1;
			_ids[i].index = -1;
			_ids[i].entryIndex = -1;
		}
	}

	void GUIDialog::clear() {
		m_Items.clear();
		_buttons.clear();
		_texts.clear();
		_images.clear();
		_numbers.clear();
		_timers.clear();
		for (int i = 0; i < 32; ++i) {
			_ids[i].id = -1;
			_ids[i].index = -1;
			_ids[i].entryIndex = -1;
		}
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
		strcpy(_name,name);
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

	int GUIDialog::createItem(const v2& position,GUIItemType type,float scale,bool centered,const Color& color) {
		GUIItem item;
		item.pos = position;
		item.type = type;
		item.scale = scale;
		item.centered = centered;
		item.color = color;
		m_Items.push_back(item);
		return m_Items.size() - 1;
	}

	GUID GUIDialog::addNumber(int id,const v2& position, int value, int length, float scale, const Color& color) {
		GUID& gid = _ids[id];
		assert(gid.id == -1);
		gid.id = id;
		// add entry
		Vector2f p = position;
		gid.entryIndex = createItem(position, GIT_NUMBERS, scale, false, color);
		GUINumber number;
		number.value = value;
		number.length = length;
		gid.index = _numbers.size();
		_numbers.push_back(number);
		return gid;
	}

	void GUIDialog::setNumber(int id, int value) {
		const GUID& gid = _ids[id];
		assert(gid.id != -1);
		GUINumber& number = _numbers[gid.index];
		number.value = value;
	}

	// -------------------------------------------------------
	// Add static image
	// -------------------------------------------------------
	GUID GUIDialog::addImage(int id, int x, int y, const Rect& textureRect, bool centered) {
		GUID& gid = _ids[id];
		assert(gid.id == -1);
		gid.id = id;
		// add entry
		Vector2f p = v2(x,y);
		gid.entryIndex = createItem(p, GIT_IMAGE, 1.0f, centered);
		GUIImage image;
		image.texture = math::buildTexture(textureRect);
		gid.index = _images.size();
		_images.push_back(image);
		return gid;
		/*

		assert(!containsItem(id));
		Vector2f p = Vector2f(x,y);
		if ( centered ) {
			p.x = renderer::getScreenWidth() * 0.5f;
		}
		GUIItem item;
		item.type = GIT_IMAGE;
		item.pos = p;
		item.id = id;
		item.texture = 
		item.centered = centered;
		item.color = Color::WHITE;
		item.scale = 1.0f;
		m_Items.push_back(item);
		return item.id;
		*/
	}

	// -------------------------------------------------------
	// update image
	// -------------------------------------------------------
	void GUIDialog::updateImage(int id, int x, int y, const Rect& textureRect, bool centered) {
		const GUID& gid = _ids[id];
		GUIItem& item = m_Items[gid.entryIndex];
		assert(item.type == GIT_IMAGE);
		GUIImage& image = _images[gid.index];
		item.centered = centered;
		item.pos = v2(x, y);
		image.texture = math::buildTexture(textureRect);
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
		//float w = textureRect.width();
		//float h = textureRect.height();
		//item.boundingRect = Rect(h * 0.5f, w * -0.5f, w, -h);
		m_Items.push_back(item);
		return item.id;
	}

	GUID GUIDialog::addTimer(int id,int x, int y, float scale, const Color& color, bool centered) {
		GUID& gid = _ids[id];
		assert(gid.id == -1);
		gid.id = id;
		Vector2f p = v2(x, y);
		gid.entryIndex = createItem(p, GIT_TIMER, scale, centered, color);
		GameTimer timer;
		gid.index = _timers.size();
		_timers.push_back(timer);
		return gid;
	}

	// -------------------------------------------------------
	// Adds a text
	// -------------------------------------------------------
	GUID GUIDialog::addText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
		GUID& gid = _ids[id];
		assert(gid.id == -1);
		gid.id = id;
		Vector2f p = v2(x,y);
		gid.entryIndex = createItem(p, GIT_TEXT, scale, centered, color);
		GUIText guiText;
		strcpy(guiText.text, text.c_str());
		gid.index = _texts.size();
		_texts.push_back(guiText);
		return gid;
		/*
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
		*/
	}

	// -------------------------------------------------------
	// Update text
	// -------------------------------------------------------
	void GUIDialog::updateText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
		const GUID& gid = _ids[id];
		GUIItem& item = m_Items[gid.entryIndex];
		assert(item.type == GIT_TEXT);
		GUIText& txt = _texts[gid.index];
		item.centered = centered;
		item.color = color;
		item.scale = scale;
		item.pos = v2(x,y);
		strcpy(txt.text, text.c_str());
	}

	// -------------------------------------------------------
	// Update text
	// -------------------------------------------------------
	void GUIDialog::updateText(int id,const std::string& text) {	
		const GUID& gid = _ids[id];
		GUIItem& item = m_Items[gid.entryIndex];
		assert(item.type == GIT_TEXT);
		GUIText& txt = _texts[gid.index];
		strcpy(txt.text, text.c_str());
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
		for (int i = 0; i < 32; ++i) {
			const GUID& gid = _ids[i];
			if ( gid.entryIndex != -1 ) {
				const GUIItem& item = m_Items[gid.entryIndex];
				if (item.type == GIT_BUTTON) {
					const GUIButton& button = _buttons[gid.index];
					Rect br = button.boundingRect;
					v2 p = item.pos;
					if (item.centered) {
						p.x = renderer::getScreenWidth() * 0.5f;
					}
					br.left += p.x;
					br.right += p.x;
					br.top += p.y;
					br.bottom += p.y;
					if (x >= br.left && x <= br.right && y <= br.top && y >= br.bottom) {
						return gid.id;
					}
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
		//for (size_t i = 0; i < m_Items.size(); ++i) {
			//GUIItem& item = m_Items[i];
			//if (item.type == GIT_BUTTON && item.id == id) {
				//item.texture = math::buildTexture(textureRect);
			//}
		//}
	}

	// -------------------------------------------------------
	// add button 
	// -------------------------------------------------------
	GUID GUIDialog::addButton(int id,float x,float y, const char* text, const Rect& textureRect, const Color& textColor, float textScale, bool centered) {
		GUID& gid = _ids[id];
		assert(gid.id == -1);
		gid.id = id;
		// add entry
		Vector2f p = v2(x, y);
		gid.entryIndex = createItem(p, GIT_BUTTON, textScale, centered, textColor);
		GUIButton button;
		strcpy(button.text, text);
		button.texture = math::buildTexture(textureRect);
		float w = textureRect.width();
		float h = textureRect.height();
		button.boundingRect = Rect(h * 0.5f, w * -0.5f, w, -h);
		gid.index = _buttons.size();
		_buttons.push_back(button);
		return gid;
		/*
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
		
		m_Items.push_back(item);
		*/
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
			char buffer[32];
			for (int i = 0; i < 32; ++i) {
				const GUID& id = _ids[i];
				if (id.entryIndex != -1) {
					const GUIItem& item = m_Items[id.entryIndex];
					v2 p = item.pos;
					if (item.type == GIT_NUMBERS) {
						const GUINumber& number = _numbers[id.index];
						string::formatInt(number.value, buffer, 32, number.length);
						v2 size = font::calculateSize(*m_BitmapFont, buffer, item.scale);
						float ty = p.y - size.y * 0.5f;
						p += v2(size.x * -0.5f, -size.y * 0.5f);
						ds::sprites::drawText(m_BitmapFont, p.x, p.y, buffer, 2.0f, item.scale, item.scale, item.color);
					}
					else if (item.type == GIT_TEXT) {
						const GUIText& text = _texts[id.index];
						v2 size = font::calculateSize(*m_BitmapFont, text.text, item.scale);
						float ty = p.y - size.y * 0.5f;
						p += v2(size.x * -0.5f, -size.y * 0.5f);
						ds::sprites::drawText(m_BitmapFont, p.x, p.y, text.text, 2.0f, item.scale, item.scale, item.color);
					}
					else if (item.type == GIT_TIMER) {
						const GameTimer& timer = _timers[id.index];
						sprintf_s(buffer, 32, "%02d:%02d", timer.getMinutes(), timer.getSeconds());
						v2 size = font::calculateSize(*m_BitmapFont, buffer, item.scale);
						float ty = p.y - size.y * 0.5f;
						p += v2(size.x * -0.5f, -size.y * 0.5f);
						ds::sprites::drawText(m_BitmapFont, p.x, p.y, buffer, 2.0f, item.scale, item.scale, item.color);
					}
					else if (item.type == GIT_IMAGE) {
						const GUIImage& image = _images[id.index];
						v2 p = item.pos;
						if (item.centered) {
							p.x = renderer::getScreenWidth() * 0.5f;
						}
						sprites::draw(p, image.texture, 0.0f, item.scale, item.scale, item.color);
					}
					else if (item.type == GIT_BUTTON) {
						const GUIButton& button = _buttons[id.index];
						v2 p = item.pos;
						if (item.centered) {
							p.x = renderer::getScreenWidth() * 0.5f;
						}
						sprites::draw(p, button.texture);
						v2 size = font::calculateSize(*m_BitmapFont, button.text, item.scale);
						float ty = p.y - size.y * 0.5f;
						p += v2(size.x * -0.5f, -size.y * 0.5f);
						ds::sprites::drawText(m_BitmapFont, p.x, p.y, button.text, 2.0f, item.scale, item.scale, item.color);
					}
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
		int id = -1;
		for (int i = 0; i < 32; ++i) {
			const GUID& gid = _ids[i];
			if (gid.id == -1 ) {
				return i;
			}
		}
		return -1;
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
						assert(id != -1);
						if (_selectedElement == 0) {
							GUID gid = addImage(id, 512, 384, Rect(0,0,50,50), true);
							addToModel(gid.id, GIT_IMAGE, "Image");
						}
						else if (_selectedElement == 1) {
							GUID gid = addText(id, 512, 384, "Text", Color::WHITE, 1.0f, true);
							addToModel(gid.id, GIT_TEXT, "Text");
						}
						else if (_selectedElement == 2) {
							GUID gid = addButton(id, 512, 384, "Text", Rect(0,0,50,50), Color::WHITE, 1.0f, true);
							addToModel(gid.id, GIT_BUTTON, "Button");
						}
						else if (_selectedElement == 4) {
							GUID gid = addNumber(id,v2(512, 384), 0, 3);
							addToModel(gid.id, GIT_NUMBERS, "Numbers");
						}
						else if (_selectedElement == 5) {
							GUID gid = addTimer(id,512, 384);
							addToModel(gid.id, GIT_TIMER, "Timer");
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
			if (gui::Button(GUI_DIALOG_ID + 14, "Export")) {
				export();
			}
			if (gui::Button(GUI_DIALOG_ID + 15, "Import")) {
				import();
			}
			gui::endGroup();
			gui::beginGroup();
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

				GUID gid = _ids[element.id];
				if (gid.entryIndex != -1) {
					GUIItem* item = &m_Items[gid.entryIndex];
					char buffer[32];
					sprintf_s(buffer, 32, "ID: %d", gid.id);
					gui::Label(GUI_DIALOG_ID + 8, buffer);
					gui::InputVec2(GUI_DIALOG_ID + 3, "Position", &item->pos);
					gui::InputFloat(GUI_DIALOG_ID + 4, "Scale", &item->scale);
					gui::InputColor(GUI_DIALOG_ID + 5, "Color", &item->color);
					gui::CheckBox(GUI_DIALOG_ID + 6, "Centered", &item->centered);
					if (element.type == GIT_NUMBERS) {
						GUINumber* number = &_numbers[gid.index];
						gui::InputInt(GUI_DIALOG_ID + 7, "Value", &number->value);
						gui::InputInt(GUI_DIALOG_ID + 8, "Length", &number->length);
					}
					else if (element.type == GIT_IMAGE) {
						GUIImage* image = &_images[gid.index];
						Rect r = image->texture.rect;
						gui::InputRect(GUI_DIALOG_ID + 7, "Texture", &r);
						image->texture = math::buildTexture(r);
					}
					else if (element.type == GIT_BUTTON) {
						GUIButton* button = &_buttons[gid.index];
						Rect r = button->texture.rect;
						gui::InputRect(GUI_DIALOG_ID + 7, "Texture", &r);
						button->texture = math::buildTexture(r);
						gui::Input(GUI_DIALOG_ID + 8, "Text", button->text, 32);
					}
					else if (element.type == GIT_TEXT) {
						GUIText* text = &_texts[gid.index];
						gui::Input(GUI_DIALOG_ID + 7, "Text", text->text, 32);
					}
				}
			}
			gui::end();
		}
	}

	void GUIDialog::saveItem(BinaryWriter& writer,int id,const GUIItem& item) {
		writer.write(id);
		writer.write(item.pos);
		writer.write(item.color);
		int cnt = 0;
		if (item.centered) {
			cnt = 1;
		}
		writer.write(cnt);
		writer.write(item.scale);
	}

	void GUIDialog::saveItem(JSONWriter& writer, int id, const GUIItem& item) {
		writer.write("id", id);
		writer.write("pos", item.pos);
		writer.write("color", item.color);
		writer.write("centered", item.centered);
		writer.write("scale", item.scale);
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
		for (int i = 0; i < 32; ++i ) {
			const GUID& gid = _ids[i];
			if (gid.entryIndex != -1) {
				const GUIItem& gi = m_Items[gid.entryIndex];
				if (gi.type == GIT_IMAGE) {
					writer.startChunk(CHNK_DLG_IMAGE, 1);
					saveItem(writer, gid.id, gi);
					const GUIImage& image = _images[gid.index];
					writer.write(image.texture.rect);
					writer.closeChunk();
				}
				else if (gi.type == GIT_BUTTON) {
					writer.startChunk(CHNK_DLG_BUTTON, 1);
					saveItem(writer, gid.id, gi);
					const GUIButton& button = _buttons[gid.index];
					writer.write(button.texture.rect);
					writer.write(button.text);
					writer.closeChunk();
				}
				else if (gi.type == GIT_TEXT) {
					writer.startChunk(CHNK_DLG_TEXT, 1);
					saveItem(writer, gid.id, gi);
					const GUIText& text = _texts[gid.index];
					writer.write(text.text);
					writer.closeChunk();
				}
				else if (gi.type == GIT_TIMER) {
					writer.startChunk(CHNK_DLG_TIMER, 1);
					saveItem(writer, gid.id, gi);
					writer.closeChunk();
				}
				else if (gi.type == GIT_NUMBERS) {
					writer.startChunk(CHNK_DLG_NUMBERS, 1);
					saveItem(writer, gid.id, gi);
					const GUINumber& number = _numbers[gid.index];
					writer.write(number.value);
					writer.write(number.length);
					writer.closeChunk();
				}
			}
		}
		writer.close();
	}

	// -------------------------------------------------------
	// save
	// -------------------------------------------------------
	void GUIDialog::export() {
		char buffer[64];
		sprintf(buffer, "content\\dialogs\\%s.json", _name);
		JSONWriter jw;
		jw.open(buffer);
		for (int i = 0; i < 32; ++i) {
			const GUID& gid = _ids[i];
			if (gid.entryIndex != -1) {
				const GUIItem& gi = m_Items[gid.entryIndex];
				if (gi.type == GIT_IMAGE) {
					jw.startCategory("image");
					saveItem(jw, gid.id, gi);
					const GUIImage& image = _images[gid.index];
					jw.write("rect", image.texture.rect);
					jw.endCategory();
				}
				else if (gi.type == GIT_BUTTON) {
					jw.startCategory("button");
					saveItem(jw, gid.id, gi);
					const GUIButton& button = _buttons[gid.index];
					jw.write("rect", button.texture.rect);
					jw.write("text", button.text);
					jw.endCategory();
				}
				else if (gi.type == GIT_TEXT) {
					jw.startCategory("text");
					saveItem(jw, gid.id, gi);
					const GUIText& text = _texts[gid.index];
					jw.write("text", text.text);
					jw.endCategory();
				}
				else if (gi.type == GIT_TIMER) {
					jw.startCategory("timer");
					saveItem(jw, gid.id, gi);
					jw.endCategory();
				}
				else if (gi.type == GIT_NUMBERS) {
					jw.startCategory("numbers");
					saveItem(jw, gid.id, gi);
					const GUINumber& number = _numbers[gid.index];
					jw.write("value", number.value);
					jw.write("length", number.length);
					jw.endCategory();
				}
			}
		}
	}

	void GUIDialog::import() {
		clear();
		_model.clear();
		char buffer[64];
		sprintf(buffer, "content\\dialogs\\%s.json", _name);
		JSONReader reader;
		if (reader.parse(buffer)) {
			std::vector<Category*> categories = reader.getCategories();
			for (size_t i = 0; i < categories.size(); ++i) {
				Category* c = categories[i];
				LOG << "name: " << c->getName();
				if (c->getName() == "image" ) {
					GUIItem item;
					int id = loadItem(c, &item);
					Rect r;
					c->getRect("rect", &r);
					GUID gid = addImage(id, item.pos.x, item.pos.y, r, item.centered);
					addToModel(gid.id, GIT_IMAGE, "Image");
				}
				else if (c->getName() == "button") {
					GUIItem item;
					int id = loadItem(c, &item);
					Rect r;
					c->getRect("rect", &r);
					std::string label = c->getProperty("text");
					GUID gid = addButton(id, item.pos.x, item.pos.y, label.c_str(), r, item.color, item.scale, item.centered);
					addToModel(gid.id, GIT_BUTTON, "Button");
				}
				else if (c->getName() == "text") {
					GUIItem item;
					int id = loadItem(c, &item);
					std::string label = c->getProperty("text");
					GUID gid = addText(id, item.pos.x, item.pos.y, label.c_str(), item.color, item.scale, item.centered);
					addToModel(gid.id, GIT_TEXT, "Text");
				}
				else if (c->getName() == "numbers") {
					GUIItem item;
					int id = loadItem(c, &item);
					int value = 0;
					c->getInt("value",&value);
					int length = 0;
					c->getInt("length",&length);
					GUID gid = addNumber(id, item.pos, value, length, item.scale, item.color);
					addToModel(gid.id, GIT_NUMBERS, "Number");
				}
				else if (c->getName() == "timer") {
					GUIItem item;
					int id = loadItem(c, &item);
					GUID gid = addTimer(id, item.pos.x, item.pos.y, item.scale, item.color, item.centered);
					addToModel(gid.id, GIT_TIMER, "Timer");
				}
			}
		}
	}

	int GUIDialog::loadItem(Category* category, GUIItem* item) {
		int id = 0;
		category->getInt("id", &id);
		category->getVector2f("pos", &item->pos);
		category->getColor("color", &item->color);
		category->getBool("centered", &item->centered);
		category->getFloat("scale", &item->scale);
		return id;
	}

	int GUIDialog::loadItem(BinaryLoader& loader, GUIItem* item) {
		int id = 0;
		loader.read(&id);
		loader.read(&item->pos);
		loader.read(&item->color);
		int cnt = 0;
		loader.read(&cnt);
		item->centered = false;
		if (cnt == 1) {
			item->centered = true;
		}
		loader.read(&item->scale);
		return id;
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
		LOGC("GUIDialog") << "loading file: " << buffer;
		int signature[] = { 0, 8, 15 };
		loader.open(buffer, signature, 3);
		while (loader.openChunk() == 0) {
			if (loader.getChunkID() == CHNK_DLG_IMAGE) {
				GUIItem item;
				int id = loadItem(loader,&item);
				Rect r;
				loader.read(&r);
				GUID gid = addImage(id, item.pos.x, item.pos.y, r, item.centered);
				addToModel(gid.id, GIT_IMAGE, "Image");
			}
			else if (loader.getChunkID() == CHNK_DLG_BUTTON) {
				GUIItem item;
				int id = loadItem(loader, &item);
				Rect r;
				loader.read(&r);
				char text[32];
				loader.read(text);
				GUID gid = addButton(id, item.pos.x, item.pos.y, text, r, item.color, item.scale, item.centered);
				addToModel(gid.id, GIT_BUTTON, "Button");
			}
			else if (loader.getChunkID() == CHNK_DLG_TEXT) {
				GUIItem item;
				int id = loadItem(loader, &item);
				char text[32];
				loader.read(text);
				GUID gid = addText(id, item.pos.x, item.pos.y, text, item.color, item.scale, item.centered);
				addToModel(gid.id, GIT_TEXT, "Text");
			}
			else if (loader.getChunkID() == CHNK_DLG_NUMBERS) {
				GUIItem item;
				int id = loadItem(loader, &item);
				int value = 0;
				loader.read(&value);
				int length = 0;
				loader.read(&length);
				GUID gid = addNumber(id, item.pos, value, length, item.scale, item.color);
				addToModel(gid.id, GIT_NUMBERS, "Number");
			}
			else if (loader.getChunkID() == CHNK_DLG_TIMER) {
				GUIItem item;
				int id = loadItem(loader, &item);
				GUID gid = addTimer(id, item.pos.x, item.pos.y, item.scale, item.color, item.centered);
				addToModel(gid.id, GIT_TIMER, "Timer");
			}
			loader.closeChunk();
		}		
	}
}
