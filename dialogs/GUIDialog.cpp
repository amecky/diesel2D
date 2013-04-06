#include "GUIDialog.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"

namespace ds {

const float INPUT_PADDING = 8.0f;
// -------------------------------------------------------
// Constructor
// -------------------------------------------------------
GUIDialog::GUIDialog(const char* name,Renderer* renderer,int material,int bitmapFont) : Node("GUIDialog",renderer,material) , m_Active(false) {
	m_MeshNode = new SimpleQuadNode("GUIQuadNode",renderer,material,100);
	m_TextNode = new TextNode("GUITextNode",renderer,material,bitmapFont);
	m_HashName = string::murmur_hash(name);
	m_SupportHover = false;
	m_SelectedInput = -1;
}


// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
GUIDialog::~GUIDialog(void) {
	delete m_MeshNode;
	delete m_TextNode;
}

// -------------------------------------------------------
// Add static image
// -------------------------------------------------------
uint32 GUIDialog::addImage(const Vec2& pos,const Rect& textureRect,float textureSize,bool centered) {
	Vec2 p = pos;
	if ( centered ) {
		p.x = ( 1024.0f - textureRect.width()) * 0.5f;
	}
	return m_MeshNode->create(textureRect.width(),textureRect.height(),p,textureRect,textureSize);
}

// -------------------------------------------------------
// Add image
// -------------------------------------------------------
uint32 GUIDialog::addImage(float y,const char* taItem) {
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	Vec2 p(0.0f,y);
	p.x = ( 1024.0f - w) * 0.5f;	
	return m_MeshNode->create(w,h,p,taItem);
}

// -------------------------------------------------------
// Add image link
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(const Vec2& pos,const Rect& textureRect,float textureSize,bool centered) {
	ImageLink link;
	Vec2 p = pos;
	if ( centered ) {
		p.x = ( 1024.0f - textureRect.width()) * 0.5f;
	}
	link.index = m_MeshNode->create(textureRect.width(),textureRect.height(),p,textureRect,textureSize);
	link.pos = p;
	float w = textureRect.width();
	float h = textureRect.height();
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	return link.index;
}

// -------------------------------------------------------
// Add image link centered vertically
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(float y,const char* taItem) {
	ImageLink link;
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	Vec2 p(0.0f,y);
	p.x = ( 1024.0f - w) * 0.5f;	
	link.index = m_MeshNode->create(w,h,p,taItem);
	link.pos = p;
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	return link.index;
}

// -------------------------------------------------------
// Add image link
// -------------------------------------------------------
uint32 GUIDialog::addImageLink(const Vec2& pos,const char* taItem,bool centered) {
	ImageLink link;
	Vec2 p = pos;
	float w = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float h = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	if ( centered ) {
		p.x = ( 1024.0f - w) * 0.5f;
	}
	link.index = m_MeshNode->create(w,h,p,taItem);
	link.pos = p;
	link.rect = Rect(p.y,p.x,w,h);
	m_ImageLinks.push_back(link);
	return link.index;
}

// -------------------------------------------------------
// Adds a text
// -------------------------------------------------------
uint32 GUIDialog::addText(const Vec2& pos,const std::string& text,const Color& color,float scale,bool centered) {
	Vec2 p = pos;
	if ( centered ) {
		Vec2 size = m_TextNode->calculateSize(text,scale);
		p.x = ( 1024.0f - size.x ) * 0.5f;
	}
	return m_TextNode->addText(p,text,color,scale);
}

// -------------------------------------------------------
// Update text
// -------------------------------------------------------
void GUIDialog::updateText(uint32 id,const std::string& text,bool centered) {
	m_TextNode->updateText(id,text);
	if (centered ) {
		const TextEntry& te = m_TextNode->getText(id);
		Vec2 p = te.pos;
		Vec2 size = m_TextNode->calculateSize(text,te.scale);
		p.x = ( 1024.0f - size.x ) * 0.5f;
		m_TextNode->setPosition(id,p);
	}
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
				updateText(input->textIndex,input->text,false);
				return true;
			}
		}
	}
	return false;
}
// -------------------------------------------------------
// Set button texture
// -------------------------------------------------------
void GUIDialog::setButtonTexture(const DialogButton& button,const char* taItem) {
	for ( size_t i = 0; i < m_ImageLinks.size(); ++i ) {
		ImageLink* link = &m_ImageLinks[i];
		if ( link->index == button.imageIndex ) {
			m_MeshNode->setTextureRect(link->index,taItem);
		}
	}
}

// -------------------------------------------------------
// Set image link texture
// -------------------------------------------------------
void GUIDialog::setImageLinkTexture(uint32 id,const char* taItem) {
	for ( size_t i = 0; i < m_ImageLinks.size(); ++i ) {
		ImageLink* link = &m_ImageLinks[i];
		if ( link->index == id ) {
			m_MeshNode->setTextureRect(link->index,taItem);
		}
	}
}

void GUIDialog::addInputField(float x,float y,const char* regularItem,const char* highlightItem,const std::string& text,const char* cursorItem,int size) {
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
}
// -----------------------------------------------------------
// Add a button which is a combination of an image and text
// -----------------------------------------------------------
DialogButton GUIDialog::addButton(float y,const char* taItem,const std::string& text,const Color& color,float scale) {
	DialogButton db;
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
		m_Renderer->drawNode(m_MeshNode);
		m_Renderer->drawNode(m_TextNode);	
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
void GUIDialog::setButtonHover(const char* regularItem,const char* highlightItem) {
	m_SupportHover = true;
	m_ButtonItem = regularItem;
	m_ButtonItemSelected = highlightItem;
}

}
