#pragma once
#include "UIDialog.h"
#include "..\addons\ScreenQuad.h"

namespace ds {

// =======================================================
// A checkbox which can be selected - no text
// =======================================================
class UICheckBox : public UIControl {

public:
	UICheckBox(UIDialog *dialog,uint32 ID,const std::string& label);
	virtual ~UICheckBox(void);
	bool isSelected(const Vector2f& mousePos);
	void setValue(bool value);
	void fillBuffer(const Vector2f& basePosition,UIBuffer& buffer);	
	Vector2f& getExtent() {
		return m_Dimension;		
	}
	void align(const Vector2f& gridPosition);
	bool getValue() const { 
		return m_Value; 
	}
	const UIControlType getType() const { 
		return UI_CONTROL_CHECKBOX; 
	}
private:
	Vector2f m_Dimension;
	Vector2f m_CalculatedSize;
	std::string m_Label;
	Rect m_BoundingBox;	
	bool m_Value;
	Vector2f m_BoxPosition;
};

}

