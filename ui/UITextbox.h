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
	bool isSelected(const Vector2f& mousePos);
	void onChar(char ascii);
	void setValue(const std::string& value);
	void fillBuffer(const Vector2f& basePosition,UIBuffer& buffer);
	void align(const Vector2f& gridPosition);
	std::string& getValue() { 
		return m_Value; 
	}
	const UIControlType getType() const { 
		return UI_CONTROL_TEXTBOX; 
	}
	Vector2f& getExtent() {
		return m_Dimension;
	}
private:
	Vector2f m_Dimension;
	Vector2f m_BoxPosition;
	Vector2f m_CalculatedSize;
	bool m_Selected;
	std::string m_Value;
	std::string m_Label;
	Rect m_BoundingBox;	
	Vector2f m_TextPos;
	std::vector<TransformedTextureColorVertex> m_Vertices;
};

}

