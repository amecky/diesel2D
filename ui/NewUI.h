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
	virtual void fillBuffer(const Vector2f& parentPosition,ScreenQuad& screenQuad) = 0;
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
	NewUIText(uint32 id,const Vector2f& pos,const std::string& txt);
	virtual ~NewUIText() {}
	void update(float elapsed);
	void fillBuffer(const Vector2f& parentPosition,ScreenQuad& screenQuad);
private:
	std::string m_Text;
	Vector2f m_Position;
	Character m_CharMapping[256];
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUITextbox : public UIWidget {

public:
	NewUITextbox(uint32 id,const Vector2f& pos,uint32 characters = 10);
	virtual ~NewUITextbox() {}
	void update(float elapsed);
	void fillBuffer(const Vector2f& parentPosition,ScreenQuad& screenQuad);
private:
	uint32 m_Size;
	Vector2f m_Position;
	Rect m_Rect;
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUILabel : public UIWidget {

public:
	NewUILabel(uint32 id,const Vector2f& pos,const std::string& text);
	virtual ~NewUILabel() {}
	void update(float elapsed);
	void fillBuffer(const Vector2f& parentPosition,ScreenQuad& screenQuad);
private:
	NewUIText* m_Text;
	Vector2f m_Position;
};
// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
class NewUIDialog : public ScreenQuad {

public:
	NewUIDiaLOGC(const char* name,const char* materialName);
	virtual ~NewUIDiaLOGC() {}
	void loadContent(ResManager& resourceManager) {}
	void update(float elapsed);
	void addTextbox(const Vector2f& pos);
	void addLabel(uint32 id,const Vector2f& pos,const std::string& text);
private:
	Vector2f m_Position;
	Vector2f m_Size;
	Rect m_Rect;
	ScreenQuad* m_ScreenQuad;
	List<UIWidget*> m_Widgets;
};


}
