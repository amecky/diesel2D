#include "UIDialog.h"
#include "UI.h"
#include "UILabel.h"
#include "UIButton.h"
#include "UITextbox.h"
#include "UICheckbox.h"
#include "UIDropDown.h"

namespace ds {

// =======================================================
// The dialog implementation
// =======================================================
UIDialog::UIDialog(const char* dialogName,const char* materialName,const char* bitmapFont,const char* layerName,const Vec2& pos,int gridSizeX,int gridSizeY) 
	: Node(dialogName,materialName,true)
	, m_BasePos(pos) 
	, m_ShowHeader(false)
	, m_MinSize(-1.0f)
	, m_MouseOffset(Vec2(0,0))
	, m_GridSizeX(gridSizeX) 
	, m_GridSizeY(gridSizeY) {

	m_BufferNode = new IVBufferNode<TransformedTextureColorVertex,TransformedTextureColorBuffer>(getName()+"IVBuffer",materialName);
	WorldLayer wl = gEngine->getWorld().getLayerByName(layerName);
	gEngine->getWorld().attachNode(m_BufferNode,wl);	
	m_Font = gEngine->getResourceManager().getBitmapFontByName(bitmapFont);
	m_Active = true;	
	m_Selected = 0;	
	m_Size = Vec2(200,200);
	m_Rect = Rect(0,0,256,256);
	// grid layout
	for ( uint32 y = 0; y < m_GridSizeY;++y ) {
		for ( uint32 x = 0; x < m_GridSizeX;++x ) {
			GridCell cell;
			cell.gx = x;
			cell.gy = y;
			cell.used = false;
			m_Cells.append(cell);
		}
	}
}

// -----------------------------------------------
//
// -----------------------------------------------
UIDialog::~UIDialog(void) {
	m_Items.deleteContents();
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::setHeader(const char* header) {
	Vec2 hp = m_BasePos;
	hp.add(8.0f,4.0f);
	//m_TextNode->setText("DialogHeader",hp,ds::Color(1.0f,1.0f,1.0f,1.0f),"%s",header);
	m_ShowHeader = true;
	//m_TextNode->calculateSize("DialogHeader",m_HeaderSize);
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::update(float elapsedTime) {
	if ( m_Selected != 0 ) {
		m_Selected->update(elapsedTime);
	}
	m_BufferNode->clear();
	//if ( m_ShowHeader ) {
		//m_BufferNode->add(m_BasePos.x+2,m_BasePos.y+2,m_Size.x-4.0f,m_HeaderSize.y+4.0f,ds::Rect(200,260,200,30));
	//}
	//m_BufferNode->add(m_BasePos.x,m_BasePos.y,m_Size.x,m_Size.y,m_Rect);
	for ( size_t i = 0; i < m_Items.num(); ++i ) {
		UIControl* control = m_Items[i];
		if ( ( m_Selected != 0 && m_Selected->getID() != control->getID() ) || m_Selected == 0 ) {
			control->fillBuffer(m_BasePos,*m_BufferNode);	
		}
	}
	for ( size_t i = 0; i < m_Items.num(); ++i ) {
		UIControl* control = m_Items[i];
		if ( m_Selected != 0 && m_Selected->getID() == control->getID() ) {
			control->fillBuffer(m_BasePos,*m_BufferNode);	
		}
	}
	m_BufferNode->rebuild();
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::setPosition(int x,int y) {
	m_BasePos = Vec2(x,y);	
	resize(m_MinSize);
	if ( m_ShowHeader ) {
		Vec2 hp = m_BasePos;
		hp.add(8.0f,4.0f);
		//TextLine* tl = m_TextNode->getText("DialogHeader");
		//tl->pos = hp;
	}
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::OnChar(char ascii,unsigned int keyState) {
	if ( m_Active ) {
		if ( m_Selected != 0 ) {
			m_Selected->onChar(ascii);
		}
	}
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::OnButtonUp(int button) {
	if ( m_Active ) {
		m_Selected = 0;
		Vec2 mousePos = Vec2(0,0);
		mousePos.x = gEngine->getMousePosX() + m_MouseOffset.x;
		mousePos.y = gEngine->getMousePosY() + m_MouseOffset.y;	
		for ( size_t i = 0; i < m_Items.num(); ++i ) {
			if ( m_Items[i]->isSelected(mousePos) ) {				
				m_Selected = m_Items[i];
				LOGC(logINFO,"UIDialog") << "Selected item " << m_Selected->getID();
				m_Items[i]->onFocus();
			}
		}
	}
}

// -----------------------------------------------
//
// -----------------------------------------------
UIControl* UIDialog::getControl(uint32 id) {
	for ( uint32 i = 0; i < m_Items.num(); ++i ) {
		if ( m_Items[i]->getID() == id ) {
			return m_Items[i];					
		}
	}
	return 0;
}

// -----------------------------------------------
//
// -----------------------------------------------
UIControl* UIDialog::getControl(uint32 id,UIControlType type) {
	for ( uint32 i = 0; i < m_Items.num(); ++i ) {
		if ( m_Items[i]->getID() == id && m_Items[i]->getType() == type ) {
			return m_Items[i];					
		}
	}
	return 0;
}

bool UIDialog::containsControl(uint32 id) {
	for ( uint32 i = 0; i < m_Items.num(); ++i ) {
		if ( m_Items[i]->getID() == id ) {
			return true;					
		}
	}
	return false;
}

UIDropDown* UIDialog::getDropDown(uint32 id) {
	return (UIDropDown*)getControl(id,UI_DROP_DOWN);
}

UIButton* UIDialog::getButton(uint32 id) {
	return (UIButton*)getControl(id,UI_CONTROL_BUTTON);
}

UICheckBox* UIDialog::getCheckbox(uint32 id) {
	return (UICheckBox*)getControl(id,UI_CONTROL_CHECKBOX);
}

UITextbox* UIDialog::getTextbox(uint32 id) {
	return (UITextbox*)getControl(id,UI_CONTROL_TEXTBOX);
}

UILabel* UIDialog::getLabel(uint32 id) {
	return (UILabel*)getControl(id,UI_CONTROL_LABEL);
}


void UIDialog::addDropDown(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,UIControlAlignment alignment) {
	UIDropDown* dropDown = new UIDropDown(this,id,label);
	m_Items.append(dropDown);
	setCellOwner(id,gridX,gridY,alignment);
}

void UIDialog::addLabel(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,UIControlAlignment alignment) {
	UILabel *gLabel = new UILabel(this,id,label);
	m_Items.append(gLabel);
	setCellOwner(id,gridX,gridY,alignment);
}

void UIDialog::addButton(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,UIControlAlignment alignment) {
	UIButton *gButton = new UIButton(this,id,label);
	m_Items.append(gButton);
	setCellOwner(id,gridX,gridY,alignment);
}

void UIDialog::addTextbox(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,const std::string& value,UIControlAlignment alignment) {
	m_Items.append(new UITextbox(this,id,label,value));
	setCellOwner(id,gridX,gridY,alignment);
}

void UIDialog::addCheckbox(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,bool value,UIControlAlignment alignment) {
	UICheckBox *gcb = new UICheckBox(this,id,label);
	m_Items.append(gcb);
	gcb->setValue(value);
	setCellOwner(id,gridX,gridY,alignment);
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::activate() {
	setState(true);
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::deactivate() {
	setState(false);
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::setState(bool active) {
	m_Selected = 0;
	if ( active ) {
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}		
	ShowCursor(active);
	//m_TextNode->setActive(active);
	m_BufferNode->setActive(active);
	m_Active = active;
	for ( size_t i = 0; i < m_Items.size(); ++i ) {
		m_Items[i]->onStateChanged(active);
	}
}

// -----------------------------------------------
//
// -----------------------------------------------
const bool UIDialog::isActive() {
	return m_Active;
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::registerListener(IUIEventListener* listener) {
	m_Listeners.append(listener);
}

// -----------------------------------------------
//
// -----------------------------------------------
void UIDialog::sendEvent(const IUIEvent& event) {
	for ( size_t i = 0; i < m_Listeners.num(); ++i ) {
		m_Listeners[i]->onUIEvent(event);
	}
}

// -----------------------------------------------
//
// -----------------------------------------------
Vec2 UIDialog::getAbsPos(const Vec2& relPos) {
	float x = m_BasePos.x + relPos.x;
	float y = m_BasePos.y + relPos.y;
	return Vec2(x,y);
}

// =================================================
// layout management
// =================================================
void UIDialog::setCellOwner(uint32 id,int gx,int gy,UIControlAlignment alignment) {
	int index = gx + gy * m_GridSizeX;
	GridCell* gc = &m_Cells[index];
	gc->controlID = id;
	gc->used = true;
	gc->alignment = alignment;
}

// -----------------------------------------------
//
// -----------------------------------------------
Vec2& UIDialog::resize(float minSize) {
	m_MinSize = minSize;
	// list to store the maximum size of each column
	List<float> xdim;
	for ( uint32 i = 0; i < m_GridSizeX; ++i ) {
		xdim.append(0.0f);
	}
	for ( uint32 i = 0; i < m_Cells.num(); ++i ) {
		GridCell* cell = &m_Cells[i];
		if ( cell->used ) {
			UIControl* control = getControl(cell->controlID);
			float xd = control->getExtent().x;
			if ( xd > xdim[cell->gx] ) {
				xdim[cell->gx] = xd;
			}
			cell->cellSize = control->getExtent();
		}
	}
	for ( uint32 i = 0; i < m_Cells.num(); ++i ) {
		GridCell* cell = &m_Cells[i];
		if ( cell->used ) {
			UIControl* control = getControl(cell->controlID);
			if ( control != 0 ) {
				float ydim = control->getExtent().y + 10.0f;
				if ( ydim < 40.0f ) {
					ydim = 40.0f;
				}
				float xd = xdim[cell->gx];
				float add = ( ydim - control->getExtent().y) * 0.5f;
				// FIXME: include alignment type			
				float px = m_BasePos.x + (float)cell->gx * xd + 10.0f;				
				float py = m_BasePos.y + (float)cell->gy * ydim + add;
				if ( m_ShowHeader ) {
					py += m_HeaderSize.y;
					py += 8.0f;
				}
				cell->cellPos = Vec2(px,py);
			}
		}
	}
	// finalize the alignment
	for ( uint32 i = 0; i < m_Cells.num(); ++i ) {
		GridCell* cell = &m_Cells[i];
		if ( cell->used ) {
			UIControl* control = getControl(cell->controlID);
			if ( control != 0 ) {				
				control->align(cell->cellPos);
			}
		}
	}
	float bottom = 10.0f;
	float right = 10.0f;
	for ( uint32 i = 0; i <m_Cells.num(); ++i ) {
		GridCell* cell = &m_Cells[i];
		if ( cell->used ) {
			float d = cell->cellSize.x + cell->cellPos.x - m_BasePos.x;
			if ( d > right ){
				right = d + 10.0f;
			}
			d = cell->cellSize.y + cell->cellPos.y - m_BasePos.y;
			if ( d > bottom ) {
				bottom = d;
			}
		}
	}
	if ( minSize > 0.0f && right < minSize ) {
		right = minSize;
	}
	bottom += 10.0f;
	m_Size.x = right;
	m_Size.y = bottom;
	update(10.0f);
	return m_Size;
}

}