#include "UIDropDown.h"
namespace ds {

// =======================================================
// 
// =======================================================
UIDropDown::UIDropDown(UIDialog *dialog,uint32 ID,const std::string& label) : UIControl(dialog,ID) , m_Label(label) , m_Selected(false) , m_Selection(0) , m_Highlight(-1) {
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),Vec2(0,0),"%s",m_Label.c_str());
	//m_Text = "Select one entry";
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID)+"_Text",Vec2(0,0),"%s",m_Label.c_str());
	//m_Dialog->getTextNode()->calculateSize(string::to_string(m_ID)+"_Text",m_CalculatedSize);

}

void UIDropDown::add(const std::string& text) {
	int index = m_Items.size();
	ModelItem mi;
	mi.text = text;
	mi.reference = string::to_string(m_ID)+"_Model_"+string::to_string(index);
	//m_Dialog->getTextNode()->setText(mi.reference,Vec2(0,0),"%s",text.c_str());
	//m_Dialog->getTextNode()->calculateSize(mi.reference,mi.size);
	if ( index == 0 ) {
		mi.active = true;
	}
	else {
		mi.active = false;
	}
	m_Items.push_back(mi);	
}

void UIDropDown::align(const Vec2& gridPosition) {
	m_Position = gridPosition;
	/*
	m_Dialog->getTextNode()->setText(string::to_string(m_ID),m_Position,"%s",m_Label.c_str());	
	m_BoxPosition.x = gridPosition.x + m_CalculatedSize.x;
	m_BoxPosition.x += 6.0f;
	m_BoxPosition.y = gridPosition.y - 4.0f;
	m_TextPosition.x = m_CalculatedSize.x + gridPosition.x + 10;
	m_TextPosition.y = gridPosition.y;	
	m_Width = 0.0f;
	m_Height = 0.0f;
	Vec2 tp = m_TextPosition;
	m_Dialog->getTextNode()->setText(string::to_string(m_ID)+"_Text",tp,"%s",m_Text.c_str());
	tp.y += m_CalculatedSize.y;
	tp.y += 2.0f;
	m_Height += m_CalculatedSize.y + 2.0f;
	for ( size_t i = 0; i < m_Items.size();++i ) {
		ModelItem* mi = &m_Items[i];		
		m_Dialog->getTextNode()->setText(mi->reference,tp,"%s",mi->text.c_str());
		mi->boundingBox.set(tp.y,tp.x,mi->size.x+tp.x,mi->size.y+tp.y);
		if ( mi->size.x > m_Width ) {
			m_Width = mi->size.x;
		}
		tp.y += mi->size.y;
		tp.y += 2.0f;
		m_Height += mi->size.y;
		m_Height += 2.0f;
		if ( i != m_Selection ) {
			m_Dialog->getTextNode()->getText(mi->reference)->active = false;
		}
	}
	m_Height += 10.0f;
	m_Width += 8.0f;
	m_BoundingBox.set(m_BoxPosition.y,m_BoxPosition.x,m_BoxPosition.x+m_Width+16.0f,m_BoxPosition.y+20.0f+4.0f);		
	*/
}

void UIDropDown::fillBuffer(const Vec2& basePosition,UIBuffer& buffer) {
	/*
	if ( m_Selected ) {
		buffer.add(m_BoxPosition.x,m_BoxPosition.y,m_Width,m_Height,Rect(240,260,200,20));
		buffer.add(m_BoxPosition.x+m_Width,m_BoxPosition.y,16.0f,16.0f,Rect(20,390,16,16));
		if ( m_Highlight != -1 ) {
			ModelItem* mi = &m_Items[m_Highlight];
			buffer.add(mi->boundingBox.left-2.0f,mi->boundingBox.top,m_Width-2.0f,mi->size.y,Rect(200,260,200,30));
		}
		
	}
	else {
		buffer.add(m_BoxPosition.x,m_BoxPosition.y,m_Width,20.0f,Rect(0,260,100,20));
		buffer.add(m_BoxPosition.x+m_Width,m_BoxPosition.y,16.0f,16.0f,Rect(20,370,16,16));
	}	
	*/
}

void UIDropDown::update(float elapsed) {
	if ( m_Selected ) {
		m_Highlight = -1;
		Vec2 mousePos = gEngine->getMousePosition();
		for ( size_t i = 0; i < m_Items.size();++i ) {
			ModelItem* mi = &m_Items[i];	
			if ( mousePos.x >= mi->boundingBox.left && mousePos.x <= mi->boundingBox.right && mousePos.y >= mi->boundingBox.top && mousePos.y <= mi->boundingBox.bottom ) {
				m_Highlight = i;
			}
		}
	}
}

bool UIDropDown::isSelected(const Vec2& mousePos) {
	Rect br = m_BoundingBox;
	//m_Selected = false;
	if ( m_Selected ) {
		m_Highlight = -1;
		for ( size_t i = 0; i < m_Items.size();++i ) {
			ModelItem* mi = &m_Items[i];	
			if ( mousePos.x >= mi->boundingBox.left && mousePos.x <= mi->boundingBox.right && mousePos.y >= mi->boundingBox.top && mousePos.y <= mi->boundingBox.bottom ) {
				m_Highlight = i;
				m_Selection = i;
				m_Text = mi->text;
				//m_Dialog->getTextNode()->setText(string::to_string(m_ID)+"_Text",m_TextPosition,"%s",m_Text.c_str());
			}
		}
	}
	if ( mousePos.x >= br.left && mousePos.x <= br.right && mousePos.y >= br.top && mousePos.y <= br.bottom && !m_Selected ) {
		m_Selected = true;
		for ( size_t i = 0; i < m_Items.size();++i ) {
			ModelItem* mi = &m_Items[i];	
			//m_Dialog->getTextNode()->getText(mi->reference)->active = true;		
		}
		return true;
	}
	if ( m_Selected ) {
		m_Selected = false;
	}
	for ( size_t i = 0; i < m_Items.size();++i ) {
		ModelItem* mi = &m_Items[i];	
		//m_Dialog->getTextNode()->getText(mi->reference)->active = false;
	}
	return false;
}

}
