#include "UIButton.h"
#include "UI.h"

namespace ds {

// =======================================================
// The button UI control implementation
// =======================================================

UIButton::UIButton(UIDialog *dialog,uint32 ID,const std::string& label) : UIControl(dialog,ID) , m_Label(label) {
	//Vector2f sp = m_Dialog->getAbsPos(pos);
	int diff = 20;
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),Vector2f(0,0),Color(1.0f,1.0f,1.0f,1.0f),"%s",label.c_str());
	//m_Dialog->getTextNode()->calculateSize(string::to_string(m_ID),m_CalculatedSize);
	// (float scale,BitmapFont* font,const std::string& text)
	m_CalculatedSize.x = (float)UI::calculateTextSize(1.0f,dialog->getBitmapFont(),label);

	m_CalculatedSize.x += 40.0f;
	//m_BoundingBox.set(sp.y,sp.x,sp.x+m_CalculatedSize.x,sp.y+25.0f);
	m_CalculatedSize.y = 25.0f;
}


UIButton::~UIButton(void) {
}

void UIButton::align(const Vector2f& gridPosition) {
	m_Vertices.clear();
	UI::fillQuad(gridPosition.x,gridPosition.y,m_CalculatedSize.x,25.0f,Rect(120,260,8,25),m_Vertices);

	float x = gridPosition.x + 8.0f;
	int complete = (int)m_CalculatedSize.x / 84;	
	int rest = m_CalculatedSize.x - complete * 84;
	LOG << "----------- size " << m_CalculatedSize.x << " complete " << complete << " rest " << rest;
	for ( int i = 0; i < complete; ++i ) {
		UI::fillQuad(x,gridPosition.y,m_CalculatedSize.x,25.0f,Rect(120,268,84,25),m_Vertices);
		x += 84.0f;
	}
	UI::fillQuad(x,gridPosition.y,m_CalculatedSize.x,25.0f,Rect(120,268,rest,25),m_Vertices);
	x += rest;
	UI::fillQuad(x,gridPosition.y,m_CalculatedSize.x,25.0f,Rect(120,354,8,25),m_Vertices);
	UI::fillText(Vector2f(gridPosition.x+8.0f,gridPosition.y+4.0f),1.0f,m_Dialog->getBitmapFont(),m_Label,Color(1.0f,1.0f,1.0f,1.0f),m_Vertices);
	
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),Vector2f(gridPosition.x+20.0f,gridPosition.y+6.0f),Color(1.0f,1.0f,1.0f,1.0f),"%s",m_Label.c_str());
	
	m_BoundingBox.set(gridPosition.y,gridPosition.x,gridPosition.x+m_CalculatedSize.x,gridPosition.y+25.0f);
	m_Position = gridPosition;
}

bool UIButton::isSelected(const Vector2f& mousePos) {
	Rect br = m_BoundingBox;
	if ( mousePos.x >= br.left && mousePos.x <= br.right && mousePos.y >= br.top && mousePos.y <= br.bottom ) {
		const BaseUIEvent be(UI_EVENT_BUTTON_CLICKED,m_ID);
		m_Dialog->sendEvent(be);
		return true;
	}
	return false;
}

void UIButton::fillBuffer(const Vector2f& basePosition,UIBuffer& buffer) {
	for ( size_t i = 0; i < m_Vertices.size();++i ) {
		buffer.add(m_Vertices[i]);
	}
}

}