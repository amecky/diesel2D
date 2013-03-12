#pragma once
#include "UIDialog.h"

namespace ds {

// =======================================================
// A text input control
// =======================================================
class UITextbox : public UIControl {

public:
	UITextbox(UIDialog *dialog,uint32 ID,const std::string& label,const std::string& value);
	~UITextbox(void);
	bool isSelected(const Vec2& mousePos);
	void onChar(char ascii);
	void setValue(const std::string& value);
	void fillBuffer(const Vec2& basePosition,UIBuffer& buffer);
	void align(const Vec2& gridPosition);
	std::string& getValue() { 
		return m_Value; 
	}
	const UIControlType getType() const { 
		return UI_CONTROL_TEXTBOX; 
	}
	Vec2& getExtent() {
		return m_Dimension;
	}
private:
	Vec2 m_Dimension;
	Vec2 m_BoxPosition;
	Vec2 m_CalculatedSize;
	bool m_Selected;
	std::string m_Value;
	std::string m_Label;
	Rect m_BoundingBox;	
	Vec2 m_TextPos;
	std::vector<TransformedTextureColorVertex> m_Vertices;
};

}

