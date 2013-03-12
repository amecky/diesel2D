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
	bool isSelected(const Vec2& mousePos);
	void setValue(bool value);
	void fillBuffer(const Vec2& basePosition,UIBuffer& buffer);	
	Vec2& getExtent() {
		return m_Dimension;		
	}
	void align(const Vec2& gridPosition);
	bool getValue() const { 
		return m_Value; 
	}
	const UIControlType getType() const { 
		return UI_CONTROL_CHECKBOX; 
	}
private:
	Vec2 m_Dimension;
	Vec2 m_CalculatedSize;
	std::string m_Label;
	Rect m_BoundingBox;	
	bool m_Value;
	Vec2 m_BoxPosition;
};

}

