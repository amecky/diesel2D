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
	Vector2f size;
};

typedef std::vector<ModelItem> ModelItems;

public:
	UIDropDown(UIDialog *dialog,uint32 ID,const std::string& label);
	virtual ~UIDropDown(void) {}
	void fillBuffer(const Vector2f& basePosition,UIBuffer& buffer);
	const UIControlType getType() const { 
		return UI_DROP_DOWN; 
	}
	Vector2f& getExtent() {
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
	bool isSelected(const Vector2f& mousePos);
	void align(const Vector2f& gridPosition);	
	void add(const std::string& text);
	void update(float elapsed);
private:
	std::string m_Label;
	Vector2f m_CalculatedSize;
	bool m_Selected;
	Vector2f m_BoxPosition;
	Vector2f m_TextPosition;
	int m_Selection;
	Rect m_BoundingBox;	
	float m_Width;
	float m_Height;
	ModelItems m_Items;
	int m_Highlight;
	std::string m_Text;
};

}


