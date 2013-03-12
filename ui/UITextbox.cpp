#include "UITextbox.h"
#include "UI.h"

namespace ds {

// =======================================================
// The text input UI control implementation
// =======================================================

UITextbox::UITextbox(UIDialog *dialog,uint32 ID,const std::string& label,const std::string& value) 
	: UIControl(dialog,ID) 
	, m_Value(value) 
	, m_Label(label)
	, m_Selected(false) {

	m_CalculatedSize.x = (float)UI::calculateTextSize(1.0f,dialog->getBitmapFont(),label);
	m_CalculatedSize.y = 25.0f;
	m_Dimension = Vec2(m_CalculatedSize.x + 65.0f,20.0f);
}

void UITextbox::align(const Vec2& gridPosition) {	
	m_Position = gridPosition;
	m_BoxPosition.x = gridPosition.x + m_CalculatedSize.x;
	m_BoxPosition.x += 6.0f;
	m_BoxPosition.y = gridPosition.y - 4.0f;
	m_TextPos.x = m_CalculatedSize.x + gridPosition.x + 14;
	m_TextPos.y = gridPosition.y + 2.0f;
	m_BoundingBox.set(m_BoxPosition.y,m_BoxPosition.x,m_BoxPosition.x+100.0f,m_BoxPosition.y+25.0f);				
	m_Vertices.clear();
	if ( m_Selected ) {
		UI::fillQuad(gridPosition.x+m_CalculatedSize.x + 10.0f,gridPosition.y,m_CalculatedSize.x,25.0f,Rect(30,260,100,25),m_Vertices);
	}
	else {
		UI::fillQuad(gridPosition.x+m_CalculatedSize.x + 10.0f,gridPosition.y,m_CalculatedSize.x,25.0f,Rect(0,260,100,25),m_Vertices);
	}
	UI::fillText(Vec2(gridPosition.x+5.0f,gridPosition.y+2.0f),1.0f,m_Dialog->getBitmapFont(),m_Label,Color(1.0f,1.0f,1.0f,1.0f),m_Vertices);
	UI::fillText(Vec2(m_TextPos.x,m_TextPos.y),1.0f,m_Dialog->getBitmapFont(),m_Value,Color(1.0f,1.0f,1.0f,1.0f),m_Vertices);
}

UITextbox::~UITextbox(void) {
}

void UITextbox::onChar(char ascii) {	
	// backspace pressed
	if ( ascii == 8 ) {
		// if there isn't anything in the user input return
		if ( m_Value.length() != 0) {
			m_Value.erase(m_Value.length()-1,1);		
		}
	}
	// enter pressed
	if ( ascii == 13) {
		// clear user input
		//m_Value = "";
	}
	if ( ascii >= 32 ) {		
		m_Value += ascii;
	}	
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),m_TextPos,Color(1.0f,1.0f,1.0f,1.0f),"%s",m_Value.c_str());
	align(m_Position);
}

void UITextbox::setValue(const std::string& value) {
	m_Value = value;
	align(m_Position);
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),m_TextPos,Color(1.0f,1.0f,1.0f,1.0f),"%s",m_Value.c_str());
}

bool UITextbox::isSelected(const Vec2& mousePos) {
	Rect br = m_BoundingBox;
	m_Selected = false;
	if ( mousePos.x >= br.left && mousePos.x <= br.right && mousePos.y >= br.top && mousePos.y <= br.bottom ) {
		m_Selected = true;
		align(m_Position);
		return true;
	}
	return false;
}

void UITextbox::fillBuffer(const Vec2& basePosition,UIBuffer& buffer) {
	for ( size_t i = 0; i < m_Vertices.size();++i ) {
		buffer.add(m_Vertices[i]);
	}
}

}
