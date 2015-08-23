#include "GUIDialog.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"

namespace ds {

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
		Sprite sp;
		sp.position = Vector2f(0,0);
		sp.texture = math::buildTexture(textureRect);
		sp.color = Color::WHITE;
		item.sprites.push_back(sp);
		item.id = id;
		m_Items.push_back(item);
		return item.id;
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
		Sprite sp;
		sp.position = Vector2f(0, 0);
		sp.texture = math::buildTexture(textureRect);
		sp.color = Color::WHITE;
		item.sprites.push_back(sp);
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
		if ( centered ) {
			Vector2f size = font::calculateSize(*m_BitmapFont,text,4,scale,scale);		
			p.x = (renderer::getScreenWidth() - size.x) * 0.5f;

		}
		GUIItem item;
		item.pos = p;
		item.type = GIT_TEXT;
		p = Vector2f(0, 0);
		font::createText(*m_BitmapFont,p,text,color,item.sprites,scale,scale);
		item.id = id;
		item.centered = centered;
		item.color = color;
		item.scale = scale;
		m_Items.push_back(item);
	}

	// -------------------------------------------------------
	// Update text
	// -------------------------------------------------------
	void GUIDialog::updateText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
		Vector2f p = Vector2f(x,y);
		if ( centered ) {
			Vector2f size = font::calculateSize(*m_BitmapFont,text,4,scale,scale);
			p.x = ( renderer::getScreenWidth() - size.x ) * 0.5f;

		}
		GUIItem* item = findByID(id);
		assert(item != 0);
		item->centered = centered;
		item->color = color;
		item->scale = scale;
		item->pos = p;
		item->sprites.clear();
		font::createText(*m_BitmapFont,Vector2f(0,0),text,color,item->sprites,scale,scale);	
	}

	// -------------------------------------------------------
	// Update text
	// -------------------------------------------------------
	void GUIDialog::updateText(int id,const std::string& text) {	
		GUIItem* item = findByID(id);
		assert(item != 0);
		Vector2f p(0, 0);// = item->pos;
		/*
		if ( item->centered ) {
			Vector2f size = font::calculateSize(*m_BitmapFont,text,4,item->scale,item->scale);
			p.x = (renderer::getScreenWidth() - size.x) * 0.5f;

		}
		item->pos.x = p.x;
		*/
		item->sprites.clear();
		font::createText(*m_BitmapFont,p,text,item->color,item->sprites,item->scale,item->scale);	
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
	// OnChar
	// -------------------------------------------------------
	bool GUIDialog::OnChar(char ascii) {
		/*
		if ( m_Active && !m_InputFields.empty() && m_SelectedInput != -1 ) {
			LOG << "dialog char " << (int)ascii;
			for ( size_t i = 0; i < m_InputFields.size(); ++i ) {
				InputField* input = &m_InputFields[i];
				if ( input->id == m_SelectedInput ) {
					if ( ascii == 8 ) {
						if ( input->text.length() > 0 ) {
							input->text.erase(input->text.length()-1,1);	
						}
					}
					else {
						if ( input->text.length() < input->size ) {
							input->text += ascii;
						}
					}
					// FIXME: adapt to new method
					//updateText(input->textIndex,input->text,false);
					return true;
				}
			}
		}
		*/
		return false;
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
				item.sprites[0].texture = math::buildTexture(textureRect);
			}
		}
	}

	// -------------------------------------------------------
	// add button 
	// -------------------------------------------------------
	void GUIDialog::addButton(int id,float x,float y, const std::string& text, const Rect& textureRect, const Color& textColor, float textScale, bool centered) {
		assert(!containsItem(id));
		if (centered) {
			x = renderer::getScreenWidth() * 0.5f;
		}
		Vector2f p = Vector2f(x, y);
		GUIItem item;
		item.pos = p;
		item.type = GIT_BUTTON;
		Sprite s;
		s.position = Vector2f(0, 0);
		s.texture = math::buildTexture(textureRect);
		item.sprites.push_back(s);		
		Vector2f size = font::calculateSize(*m_BitmapFont, text, textScale);
		float ty = y - size.y * 0.5f;
		p = Vector2f(size.x * -0.5f, -size.y * 0.5f);
		font::createText(*m_BitmapFont, p, text, textColor, item.sprites, textScale, textScale);
		item.id = id;
		item.centered = true;
		item.color = Color::WHITE;
		item.scale = 1.0f;
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
				for ( size_t j =0; j < gi->sprites.size(); ++j ) {
					sprites::draw(gi->sprites[j].position + gi->pos, gi->sprites[j].texture, gi->sprites[j].rotation, gi->sprites[j].scale.x, gi->sprites[j].scale.y, gi->sprites[j].color);
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

	// -------------------------------------------------------
	// Find by id
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
				addButton(id,p.x,p.y,txt,r,c,1.0f,centered);
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
			}		
			loader->closeChunk();
		}	
	}

	

}
