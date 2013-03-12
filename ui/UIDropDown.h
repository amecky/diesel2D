#pragma once
#include "UIDialog.h"
#include "..\addons\ScreenQuad.h"
#include <vector>
#include <string>

namespace ds {

// =======================================================
// 
// =======================================================
class UIDropDown : public UIControl {

struct ModelItem {
	std::string text;
	std::string reference;
	bool active;
	Rect boundingBox;
	Vec2 size;
};

typedef std::vector<ModelItem> ModelItems;

public:
	UIDropDown(UIDialog *dialog,uint32 ID,const std::string& label);
	virtual ~UIDropDown(void) {}
	void fillBuffer(const Vec2& basePosition,UIBuffer& buffer);
	const UIControlType getType() const { 
		return UI_DROP_DOWN; 
	}
	Vec2& getExtent() {
		return m_CalculatedSize;
	}
	std::string& getSelectedValue() {
		if ( m_Highlight != -1 ) {
			return m_Items[m_Highlight].text;
		}
		return m_Items[0].text;
	}
	int getSelectedIndex() {
		return m_Highlight;
	}
	bool isSelected(const Vec2& mousePos);
	void align(const Vec2& gridPosition);	
	void add(const std::string& text);
	void update(float elapsed);
private:
	std::string m_Label;
	Vec2 m_CalculatedSize;
	bool m_Selected;
	Vec2 m_BoxPosition;
	Vec2 m_TextPosition;
	int m_Selection;
	Rect m_BoundingBox;	
	float m_Width;
	float m_Height;
	ModelItems m_Items;
	int m_Highlight;
	std::string m_Text;
};

}


