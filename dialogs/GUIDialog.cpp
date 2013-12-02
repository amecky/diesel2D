#include "GUIDialog.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"

namespace ds {

const float INPUT_PADDING = 8.0f;
// -------------------------------------------------------
// Init
// -------------------------------------------------------
void GUIDialog::init(const char* name,const DialogID& id,SpriteBatch* spriteBatch,const BitmapFont& bitmapFont) {	
	m_Batch = spriteBatch;
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
uint32 GUIDialog::addImage(const Vec2& pos,const Rect& textureRect,bool centered) {
	Vec2 p = pos;
	if ( centered ) {
		p.x = 512.0f;
	}
	GUIItem item;
	Sprite sp;
	sp.position = p;
	sp.textureRect = textureRect;
	sp.color = Color::WHITE;
	item.sprites.push_back(sp);
	item.id = m_Index;
	++m_Index;
	m_Items.push_back(item);
	return item.id;
}

// -------------------------------------------------------
// Add image
// -------------------------------------------------------
uint32 GUIDialog::addImage(float y,const char* taItem) {
	/*
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	Vec2 p(0.0f,y);
	p.x = ( 1024.0f - w) * 0.5f;	
	*/
	return -1;//m_MeshNode->create(w,h,p,taItem);
}

// -------------------------------------------------------
// Add image link
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(const Vec2& pos,const Rect& textureRect,bool centered) {
	ImageLink link;
	Vec2 p = pos;
	if ( centered ) {
		p.x = 512.0f;
	}
	link.index = addImage(pos,textureRect,centered);	
	link.pos = p;	
	float w = textureRect.width();
	float h = textureRect.height();
	float x = ( 1024.0f - textureRect.width()) * 0.5f;
	float y = 768.0f - p.y - h * 0.5f;
	link.rect = Rect(y,x,w,h);
	m_ImageLinks.push_back(link);
	return link.index;
}

// -------------------------------------------------------
// Add image link centered vertically
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(float y,const char* taItem) {
	ImageLink link;
	/*
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	Vec2 p(0.0f,y);
	p.x = ( 1024.0f - w) * 0.5f;	
	//link.index = m_MeshNode->create(w,h,p,taItem);
	link.pos = p;
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	*/
	return link.index;
}

// -------------------------------------------------------
// Add image link
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(const Vec2& pos,const char* taItem,bool centered) {
	ImageLink link;
	/*
	Vec2 p = pos;
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	if ( centered ) {
		p.x = ( 1024.0f - w) * 0.5f;
	}
	//link.index = m_MeshNode->create(w,h,p,taItem);
	link.pos = p;
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	*/
	return link.index;
}

// -------------------------------------------------------
// Adds a text
// -------------------------------------------------------
uint32 GUIDialog::addText(const Vec2& pos,const std::string& text,const Color& color,float scale,bool centered) {
	Vec2 p = pos;
	if ( centered ) {
		Vec2 size = font::calculateSize(m_BitmapFont,text,scale);
		p.x = ( 1024.0f - size.x ) * 0.5f;

	}
	GUIItem item;
	font::createText(m_BitmapFont,p,text,color,item.sprites,scale,scale);
	item.id = m_Index;
	++m_Index;
	m_Items.push_back(item);
	return item.id;
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void GUIDialog::updateText(uint32 id,const Vec2& pos,const std::string& text,const Color& color,float scale,bool centered) {
	Vec2 p = pos;
	if ( centered ) {
		Vec2 size = font::calculateSize(m_BitmapFont,text,scale);
		p.x = ( 1024.0f - size.x ) * 0.5f;

	}
	GUIItem* item = &m_Items[id];
	
	item->sprites.clear();
	font::createText(m_BitmapFont,p,text,color,item->sprites,scale,scale);	
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
	if ( !m_InputFields.empty() && down ) {
		int selected = -1;
		for ( size_t i = 0; i < m_InputFields.size(); ++i ) {
			InputField* input = &m_InputFields[i];
			for ( size_t i = 0; i < m_ImageLinks.size(); ++i ) {
				ImageLink* link = &m_ImageLinks[i];
				if ( link->index == input->imageIndex ) {
					Rect br = link->rect;
					if ( x >= br.left && x <= br.right && y >= br.top && y <= br.bottom ) {
						selected = input->id;
					}		
				}
			}
		}
		if ( selected == -1 && m_SelectedInput != -1 ) {
			LOG(logINFO) << "deselected input " << selected;			
			setInputFieldState(m_SelectedInput,false);
			m_SelectedInput = -1;
		}
		if ( selected != -1 ) {
			LOG(logINFO) << "Input " << selected;
			m_SelectedInput = selected;
			setInputFieldState(selected,true);
		}
	}
	return -1;
}

void GUIDialog::setInputFieldState(uint32 id,bool selected) {
	for ( size_t i = 0; i < m_InputFields.size(); ++i ) {
		InputField* input = &m_InputFields[i];
		if ( input->id == id ) {
			if ( selected ) {
				setImageLinkTexture(input->imageIndex,input->highlightItem);
			}
			else {
				setImageLinkTexture(input->imageIndex,input->regularItem);
			}
		}
	}
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
void GUIDialog::setButtonTexture(const DialogButton& button,const Rect& textureRect) {
	for ( size_t i = 0; i < m_ImageLinks.size(); ++i ) {
		ImageLink* link = &m_ImageLinks[i];
		if ( link->index == button.imageIndex ) {
			GUIItem* item = &m_Items[link->index];
			for ( size_t j = 0; j < item->sprites.size(); ++j ) {
				item->sprites[j].textureRect = textureRect;
			}
		}
	}
}

// -------------------------------------------------------
// Set image link texture
// -------------------------------------------------------
void GUIDialog::setImageLinkTexture(uint32 id,const char* taItem) {
	/*
	for ( size_t i = 0; i < m_ImageLinks.size(); ++i ) {
		ImageLink* link = &m_ImageLinks[i];
		if ( link->index == id ) {
			m_MeshNode->setTextureRect(link->index,taItem);
		}
	}
	*/
}

void GUIDialog::addInputField(float x,float y,const char* regularItem,const char* highlightItem,const std::string& text,const char* cursorItem,int size) {
	/*
	InputField input;
	input.size = size;
	input.text = text;
	input.regularItem = regularItem;
	input.highlightItem = highlightItem;
	// calculate size of image
	std::string tmp = "";
	for ( int i = 0; i < size; ++i ) {
		tmp += "A";
	}
	ds::Vec2 textDim = m_TextNode->calculateSize(tmp);
	LOG(logINFO) << "text dim " << textDim.x << " " << textDim.y;
	ImageLink link;	
	Vec2 p(0.0f,y);
	p.x = ( 1024.0f - textDim.x) * 0.5f;	
	float w = textDim.x + INPUT_PADDING * 2.0f;
	float h = textDim.y + INPUT_PADDING * 2.0f;
	link.index = m_MeshNode->create(w,h,p,regularItem);
	link.pos = p;	
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	input.imageIndex = link.index;
	float ty = y + ( h - textDim.y) * 0.5f;
	float tx = p.x + INPUT_PADDING;
	input.textIndex = addText(ds::Vec2(tx,ty),text,Color::WHITE,1.0f,false);
	uint32 idx = m_Buttons.size();
	input.id = idx;
	m_InputFields.push_back(input);
	*/
}

const DialogButton& GUIDialog::addButton(float y,const std::string& text,const Rect& textureRect,const Color& textColor,float textScale) {
	DialogButton db;
	db.imageIndex = addImageLink(Vec2(0.0f,y),textureRect);
	//y = y - textureRect.height() * 0.5f;
	Vec2 size = font::calculateSize(m_BitmapFont,text,textScale);
	float ty = y - size.y * 0.5f;
	db.textIndex = addText(Vec2(100.0f,ty),text,textColor,textScale,true);
	/*
	ImageLink link;
	Vec2 p(0.0f,y);
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	p.x = ( 1024.0f - w) * 0.5f;	
	link.index = m_MeshNode->create(w,h,p,taItem);
	link.pos = p;	
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	db.imageIndex = link.index;
	Vec2 ts = m_TextNode->calculateSize(text);
	// FIXME: calculate scale as well
	float ty = y + ( h - ts.y) * 0.5f;
	float tx = p.x + ( w - ts.x ) * 0.5f;
	db.textIndex = addText(ds::Vec2(tx,ty),text,color,scale,false);
	*/
	uint32 idx = m_Buttons.size();
	db.id = idx;
	m_Buttons.push_back(db);
	return m_Buttons[idx];
}
// -----------------------------------------------------------
// Add a button which is a combination of an image and text
// -----------------------------------------------------------
DialogButton GUIDialog::addButton(float y,const char* taItem,const std::string& text,const Color& color,float scale) {
	DialogButton db;
	/*
	ImageLink link;
	Vec2 p(0.0f,y);
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	p.x = ( 1024.0f - w) * 0.5f;	
	link.index = m_MeshNode->create(w,h,p,taItem);
	link.pos = p;	
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	db.imageIndex = link.index;
	Vec2 ts = m_TextNode->calculateSize(text);
	// FIXME: calculate scale as well
	float ty = y + ( h - ts.y) * 0.5f;
	float tx = p.x + ( w - ts.x ) * 0.5f;
	db.textIndex = addText(ds::Vec2(tx,ty),text,color,scale,false);
	uint32 idx = m_Buttons.size();
	db.id = idx;
	m_Buttons.push_back(db);
	*/
	return db;
}

// -------------------------------------------------------
// Activate
// -------------------------------------------------------
void GUIDialog::activate() {
	m_Active = true;
	if ( m_SupportHover ) {
		for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
			DialogButton* db = &m_Buttons[i];		
			setButtonTexture(*db,m_ButtonItem);
		}
		m_Selected = -1;
	}
	m_SelectedInput = -1;
	for ( size_t i = 0; i < m_InputFields.size(); ++i ) {
		InputField* input = &m_InputFields[i];
		setImageLinkTexture(input->imageIndex,input->regularItem);		
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
				m_Batch->draw(gi->sprites[j]);
			}
		}
	}
}

// -------------------------------------------------------
// Update Mouse pos and set button textures if enabled
// -------------------------------------------------------
void GUIDialog::updateMousePos(const ds::Vec2& mousePos) {
	if ( m_SupportHover ) {
		int ret = onButton(0,mousePos.x,mousePos.y,false);	
		for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
			DialogButton* db = &m_Buttons[i];		
			if ( db->id == ret && ret != m_Selected ) {
				setButtonTexture(*db,m_ButtonItemSelected);
			}
		}
		if ( ret == -1 && m_Selected != -1 ) {
			for ( size_t i = 0; i < m_Buttons.size(); ++i ) {
				DialogButton* db = &m_Buttons[i];		
				if ( m_Selected == db->id ) {
					setButtonTexture(*db,m_ButtonItem);
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

}
