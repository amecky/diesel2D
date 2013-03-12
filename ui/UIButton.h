#pragma once
#include "UIDialog.h"

namespace ds {

// =======================================================
// A button control
// =======================================================
class UIButton : public UIControl {

public:
	UIButton(UIDialog *dialog,uint32 ID,const std::string& label);
	virtual ~UIButton(void);
	bool isSelected(const Vec2& mousePos);
	void fillBuffer(const Vec2& basePosition,UIBuffer& buffer);
	const UIControlType getType() const { 
		return UI_CONTROL_BUTTON; 
	}
	Vec2& getExtent() {
		return m_CalculatedSize;
	}
	void align(const Vec2& gridPosition);
private:
	std::string m_Label;
	Rect m_BoundingBox;
	Vec2 m_CalculatedSize;
	std::vector<TransformedTextureColorVertex> m_Vertices;
};

}

