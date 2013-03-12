#pragma once
#include "UIDialog.h"
#include "..\addons\ScreenQuad.h"

namespace ds {

// =======================================================
// A static label showing a text string
// =======================================================
class UILabel : public UIControl {

public:
	UILabel(UIDialog *dialog,uint32 ID,const std::string& label);
	virtual ~UILabel(void) {}
	void fillBuffer(const Vec2& basePosition,UIBuffer& buffer);
	const UIControlType getType() const { 
		return UI_CONTROL_LABEL; 
	}
	Vec2& getExtent() {
		return m_CalculatedSize;
	}
	void align(const Vec2& gridPosition);
	void setText(const std::string& label);
	void setText(char* format,...);
	void setText(char* format,va_list args);
private:
	std::string m_Label;
	Vec2 m_CalculatedSize;
	std::vector<TransformedTextureColorVertex> m_Vertices;
};

}

