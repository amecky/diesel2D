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
	void fillBuffer(const Vector2f& basePosition,UIBuffer& buffer);
	const UIControlType getType() const { 
		return UI_CONTROL_LABEL; 
	}
	Vector2f& getExtent() {
		return m_CalculatedSize;
	}
	void align(const Vector2f& gridPosition);
	void setText(const std::string& label);
	void setText(char* format,...);
	void setText(char* format,va_list args);
private:
	std::string m_Label;
	Vector2f m_CalculatedSize;
	std::vector<TransformedTextureColorVertex> m_Vertices;
};

}

