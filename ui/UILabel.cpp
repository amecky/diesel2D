#include "UILabel.h"
#include "UI.h"

namespace ds {

// =======================================================
// The label UI control implementation
// =======================================================
UILabel::UILabel(UIDialog *dialog,uint32 ID,const std::string& label) : UIControl(dialog,ID) , m_Label(label) {
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),Vector2f(0,0),"%s",m_Label.c_str());
	//m_Dialog->getTextNode()->calculateSize(string::to_string(m_ID),m_CalculatedSize);
	m_CalculatedSize.x = (float)UI::calculateTextSize(1.0f,dialog->getBitmapFont(),label);
	m_CalculatedSize.y = 25.0f;
	LOG << "------- Label " << ID << " size " << m_CalculatedSize.x << " " << m_CalculatedSize.y;

}

void UILabel::align(const Vector2f& gridPosition) {
	m_Position = gridPosition;
	m_Vertices.clear();
	UI::fillText(Vector2f(gridPosition.x+5.0f,gridPosition.y+2.0f),1.0f,m_Dialog->getBitmapFont(),m_Label,Color(1.0f,1.0f,1.0f,1.0f),m_Vertices);

	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),m_Position,"%s",m_Label.c_str());
}

void UILabel::setText(const std::string& label) {
	m_Label = label;	
	//m_Dialog->getTextNode()->setText(string::to_string(m_ID),m_Position,"%s",m_Label.c_str());
	//m_Dialog->getTextNode()->calculateSize(string::to_string(m_ID),m_CalculatedSize);
	m_Vertices.clear();
	UI::fillText(Vector2f(m_Position.x+5.0f,m_Position.y+2.0f),1.0f,m_Dialog->getBitmapFont(),m_Label,Color(1.0f,1.0f,1.0f,1.0f),m_Vertices);
}

void UILabel::setText(char* format,...) {
	va_list args;
	va_start(args,format);		
	setText(format,args);
	va_end(args);
}

void UILabel::setText(char* format,va_list args) {
	char buffer[1024];
	int written = vsnprintf_s(buffer,1023,1024,format,args);	
	buffer[1023] = '\0';		
	m_Label = std::string(buffer);
	setText(m_Label);
}

void UILabel::fillBuffer(const Vector2f& basePosition,UIBuffer& buffer) {
	for ( size_t i = 0; i < m_Vertices.size();++i ) {
		buffer.add(m_Vertices[i]);
	}
}

}
