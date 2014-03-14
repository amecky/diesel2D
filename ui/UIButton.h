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
	bool isSelected(const Vector2f& mousePos);
	void fillBuffer(const Vector2f& basePosition,UIBuffer& buffer);
	const UIControlType getType() const { 
		return UI_CONTROL_BUTTON; 
	}
	Vector2f& getExtent() {
		return m_CalculatedSize;
	}
	void align(const Vector2f& gridPosition);
private:
	std::string m_Label;
	Rect m_BoundingBox;
	Vector2f m_CalculatedSize;
	std::vector<TransformedTextureColorVertex> m_Vertices;
};

}

