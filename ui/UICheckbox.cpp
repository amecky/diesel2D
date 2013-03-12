#include "UICheckbox.h"

namespace ds {

// =======================================================
// The checkbox UI control implementation
// =======================================================
UICheckBox::UICheckBox(UIDialog *dialog,uint32 ID,const std::string& label) : UIControl(dialog,ID) , m_Label(label) {
	//m_Dialog->getTextNode()->setText("Label"+string::to_string(m_ID),Vec2(0,0),Color(1.0f,1.0f,1.0f,1.0f),"%s",label.c_str());
	//m_Dialog->getTextNode()->calculateSize("Label"+string::to_string(m_ID),m_CalculatedSize);
	m_Value = false;
	m_Dimension = Vec2(m_CalculatedSize.x + 28.0f,14.0f);
}

void UICheckBox::align(const Vec2& gridPosition) {
	//m_Dialog->getTextNode()->setText("Label"+string::to_string(m_ID),Vec2(gridPosition.x,gridPosition.y),Color(1.0f,1.0f,1.0f,1.0f),"%s",m_Label.c_str());
	m_BoxPosition.x = gridPosition.x + m_CalculatedSize.x;
	m_BoxPosition.x += 6.0f;
	m_BoxPosition.y = gridPosition.y;
	m_BoundingBox.set(m_BoxPosition.y,m_BoxPosition.x,m_BoxPosition.x+13.0f,m_BoxPosition.y+14.0f);		
}


UICheckBox::~UICheckBox(void) {
}

void UICheckBox::setValue(bool value) {
	m_Value = value;	
}

bool UICheckBox::isSelected(const Vec2& mousePos) {
	Rect br = m_BoundingBox;	
	if ( mousePos.x >= br.left && mousePos.x <= br.right && mousePos.y >= br.top && mousePos.y <= br.bottom ) {
		if ( m_Value ) {
			m_Value = false;
		}
		else {
			m_Value = true;
		}
		return true;
	}
	return false;
}

void UICheckBox::fillBuffer(const Vec2& basePosition,UIBuffer& buffer) {
	/*
	if ( m_Value ) {
		buffer.add(m_BoxPosition.x,m_BoxPosition.y,13.0f,14.0f,Rect(0,390,13,14));
	}
	else {
		buffer.add(m_BoxPosition.x,m_BoxPosition.y,13.0f,14.0f,Rect(0,370,13,14));
	}
	*/
}

}
