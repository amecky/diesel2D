#pragma once
#include "..\addons\ScreenQuad.h"
#include "..\lib\container\List.h"
#include <string>

const uint32 CHARACTER_WIDTH = 16;

namespace ds {
	// ---------------------------------------------------------------
	//
	// ---------------------------------------------------------------
class UIWidget {

public:
	UIWidget(uint32 id) : m_ID(id) {}
	virtual ~UIWidget() {}
	virtual void update(float elapsed) = 0;
	virtual void fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad) = 0;
	uint32 getID() const {
		return m_ID;
	}
protected:
	uint32 m_ID;
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUIText : public UIWidget {

struct Character {
	Rect textureRect;
	uint32 charName;
	bool supported;
};

public:
	NewUIText(uint32 id,const Vec2& pos,const std::string& txt);
	virtual ~NewUIText() {}
	void update(float elapsed);
	void fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad);
private:
	std::string m_Text;
	Vec2 m_Position;
	Character m_CharMapping[256];
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUITextbox : public UIWidget {

public:
	NewUITextbox(uint32 id,const Vec2& pos,uint32 characters = 10);
	virtual ~NewUITextbox() {}
	void update(float elapsed);
	void fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad);
private:
	uint32 m_Size;
	Vec2 m_Position;
	Rect m_Rect;
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUILabel : public UIWidget {

public:
	NewUILabel(uint32 id,const Vec2& pos,const std::string& text);
	virtual ~NewUILabel() {}
	void update(float elapsed);
	void fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad);
private:
	NewUIText* m_Text;
	Vec2 m_Position;
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUIDialog : public ScreenQuad {

public:
	NewUIDialog(const char* name,const char* materialName);
	virtual ~NewUIDialog() {}
	void loadContent(ResManager& resourceManager) {}
	void update(float elapsed);
	void addTextbox(const Vec2& pos);
	void addLabel(uint32 id,const Vec2& pos,const std::string& text);
private:
	Vec2 m_Position;
	Vec2 m_Size;
	Rect m_Rect;
	ScreenQuad* m_ScreenQuad;
	List<UIWidget*> m_Widgets;
};


}
