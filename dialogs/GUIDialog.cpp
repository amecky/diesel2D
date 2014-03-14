#include "GUIDialog.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\FileWatcher.h"

namespace ds {

const float INPUT_PADDING = 8.0f;
// -------------------------------------------------------
// Init
// -------------------------------------------------------
void GUIDialog::init(const char* name,const DialogID& id,Renderer* renderer,BitmapFont* bitmapFont) {	
	m_Renderer = renderer;
	m_ID = id;
	m_BitmapFont = bitmapFont;
	m_Active = false;
	m_HashName = string::murmur_hash(name);
	m_SupportHover = false;
	m_SelectedInput = -1;
	m_Index = 0;
}


// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
GUIDialog::~GUIDialog(void) {
}

// -------------------------------------------------------
// Add static image
// -------------------------------------------------------
uint32 GUIDialog::addImage(int x,int y,const Rect& textureRect,bool centered) {
	Vector2f p = Vector2f(x,y);
	if ( centered ) {
		p.x = m_ScreenWidth * 0.5f;
	}
	GUIItem item;
	SpriteObject sp;
	sp.setPosition(p);
	sp.setTextureRect(textureRect);
	sp.setColor(Color::WHITE);
	item.sprites.push_back(sp);
	item.id = m_Index;
	++m_Index;
	m_Items.push_back(item);
	return item.id;
}

// -------------------------------------------------------
// Add image link
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(int x,int y,const Rect& textureRect,bool centered) {
	ImageLink link;
	Vector2f p = Vector2f(x,y);
	if ( centered ) {
		p.x = m_ScreenWidth * 0.5f;
	}
	link.index = addImage(x,y,textureRect,centered);	
	link.pos = p;	
	float w = textureRect.width();
	float h = textureRect.height();
	float rx = ( m_ScreenWidth - textureRect.width()) * 0.5f;
	float ry = m_ScreenHeight - p.y - h * 0.5f;
	link.rect = Rect(ry,rx,w,h);
	m_ImageLinks.push_back(link);
	return link.index;
}

// -------------------------------------------------------
// Adds a text
// -------------------------------------------------------
void GUIDialog::addText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
	Vector2f p = Vector2f(x,y);
	if ( centered ) {
		Vector2f size = font::calculateSize(*m_BitmapFont,text,4,scale,scale);		
		p.x = ( m_ScreenWidth - size.x ) * 0.5f;

	}
	GUIItem item;
	font::createText(*m_BitmapFont,p,text,color,item.sprites,scale,scale);
	item.id = id;
	item.centered = centered;
	item.color = color;
	item.scale = scale;
	item.pos = p;
	m_Items.push_back(item);
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void GUIDialog::updateText(int id,int x,int y,const std::string& text,const Color& color,float scale,bool centered) {
	Vector2f p = Vector2f(x,y);
	if ( centered ) {
		Vector2f size = font::calculateSize(*m_BitmapFont,text,4,scale,scale);
		p.x = ( m_ScreenWidth - size.x ) * 0.5f;

	}
	GUIItem* item = findByID(id);
	assert(item != 0);
	item->centered = centered;
	item->color = color;
	item->scale = scale;
	item->pos = p;
	item->sprites.clear();
	font::createText(*m_BitmapFont,p,text,color,item->sprites,scale,scale);	
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void GUIDialog::updateText(int id,const std::string& text) {	
	GUIItem* item = findByID(id);
	assert(item != 0);
	Vector2f p = item->pos;
	if ( item->centered ) {
		Vector2f size = font::calculateSize(*m_BitmapFont,text,4,item->scale,item->scale);
		p.x = ( m_ScreenWidth - size.x ) * 0.5f;

	}
	item->pos.x = p.x;
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
	for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
		DialogButton* db = &m_Buttons[i];		
		for ( size_t i = 0; i < m_ImageLinks.size(); ++i ) {
			ImageLink* link = &m_ImageLinks[i];
			if ( link->index == db->imageIndex ) {
				Rect br = link->rect;
				if ( x >= br.left && x <= br.right && y >= br.top && y <= br.bottom ) {
					return db->id;
				}		
			}
		}
	}	
	return -1;
}

// -------------------------------------------------------
// OnChar
// -------------------------------------------------------
bool GUIDialog::OnChar(char ascii) {
	if ( m_Active && !m_InputFields.empty() && m_SelectedInput != -1 ) {
		LOG(logINFO) << "dialog char " << (int)ascii;
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
	return false;
}
// -------------------------------------------------------
// Set button texture
// -------------------------------------------------------
void GUIDialog::setButtonTexture(int id,const Rect& textureRect) {
	for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
		DialogButton* button = &m_Buttons[i];
		if ( button->id == id ) {		
			GUIItem* item = &m_Items[button->imageIndex];
			for ( size_t j = 0; j < item->sprites.size(); ++j ) {
				item->sprites[j].setTextureRect(textureRect);
			}
		}
	}
}

void GUIDialog::addButton(int id,float y,const std::string& text,const Rect& textureRect,const Color& textColor,float textScale) {
	DialogButton db;
	db.imageIndex = addImageLink(0,y,textureRect);
	//y = y - textureRect.height() * 0.5f;
	Vector2f size = font::calculateSize(*m_BitmapFont,text,textScale);
	float ty = y - size.y * 0.5f;
	int textID = findFreeID();
	addText(textID,100,ty,text,textColor,textScale,true);	
	db.textIndex = textID;
	db.id = id;
	m_Buttons.push_back(db);
}

// -------------------------------------------------------
// Activate
// -------------------------------------------------------
void GUIDialog::activate() {
	m_Active = true;
	if ( m_SupportHover ) {
		for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
			DialogButton* db = &m_Buttons[i];		
			setButtonTexture(db->id,m_ButtonItem);
		}
		m_Selected = -1;
	}
	m_SelectedInput = -1;	
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
				m_Renderer->draw(gi->sprites[j]);
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
		for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
			DialogButton* db = &m_Buttons[i];		
			if ( db->id == ret && ret != m_Selected ) {
				setButtonTexture(db->id,m_ButtonItemSelected);
			}
		}
		if ( ret == -1 && m_Selected != -1 ) {
			for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
				DialogButton* db = &m_Buttons[i];		
				if ( m_Selected == db->id ) {
					setButtonTexture(db->id,m_ButtonItem);
				}
			}
		}
		m_Selected = ret;
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
// Load
// -------------------------------------------------------
void GUIDialog::load(const char* fileName) {
	char buffer[256];
	sprintf(buffer,"content\\dialogs\\%s.json",fileName);
	JSONReader reader;
	if ( reader.parse(buffer) ) {		
		loadDialogFromJSON(reader);		
		gFileWatcher->registerFile(buffer,this);
	}
}

void GUIDialog::reload(const char* fileName) {
	JSONReader reader;
	if ( reader.parse(fileName) ) {	
		clear();
		loadDialogFromJSON(reader);		
	}
}

// -------------------------------------------------------
// Internal load dialog from json file
// -------------------------------------------------------
void GUIDialog::loadDialogFromJSON(JSONReader& reader) {
	std::vector<Category*> categories = reader.getCategories();
	for ( size_t i = 0; i < categories.size(); ++i ) {
		Category* c = categories[i];
		if ( c->getName() == "image" ) {
			Rect r;
			c->getRect("rect",&r);
			Vector2f p = c->getVector2f("pos");
			bool centered = true;
			c->getBool("centered",&centered);
			addImage(p.x,p.y,r,centered);
		}
		if ( c->getName() == "button" ) {
			int id = c->getInt(0,"id");
			Rect r;
			c->getRect("rect",&r);
			Vector2f p = c->getVector2f("pos");
			std::string txt = c->getProperty("text");
			// FIXME: read centered
			addButton(id,p.y,txt,r);
		}
		if ( c->getName() == "text" ) {
			int id = c->getInt(0,"id");
			Vector2f p = c->getVector2f("pos");
			float scale = 1.0f;
			c->getFloat("scale",&scale);
			std::string txt = c->getProperty("text");
			Color clr = Color::WHITE;
			c->getColor("color",&clr);
			bool centered = true;
			c->getBool("centered",&centered);
			addText(id,p.x,p.y,txt,clr,scale,centered);
		}
	}
}

}
