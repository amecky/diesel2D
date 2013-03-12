#pragma once
#include "..\nodes\Node.h"
#include "..\renderer\Renderer.h"
#include "..\nodes\SimpleQuadNode.h"
#include "..\nodes\TextNode.h"
#include <vector>

namespace ds {

struct DialogButton {
	uint32 id;
	uint32 imageIndex;
	uint32 textIndex;
};

struct InputField {
	uint32 id;
	const char* regularItem;
	const char* highlightItem;
	uint32 imageIndex;
	uint32 textIndex;
	std::string text;
	uint32 cursorIndex;
	int size;
};

class GUIDialog : Node {

struct ImageLink {
	uint32 index;
	Vec2 pos;
	Rect rect;
};



typedef std::vector<ImageLink> ImageLinks;
typedef std::vector<DialogButton> DialogButtons;
typedef std::vector<InputField> InputFields;

public:
	GUIDialog(const char* name,Renderer* renderer,int material,int bitmapFont);
	~GUIDialog(void);
	virtual void init() {}
	virtual void render();
	void setButtonHover(const char* regularItem,const char* highlightItem);
	uint32 addImage(float y,const char* taItem);
	uint32 addImage(const Vec2& pos,const Rect& textureRect,float textureSize,bool centered = true);
	uint32 addImageLink(const Vec2& pos,const Rect& textureRect,float textureSize,bool centered = true);
	uint32 addImageLink(const Vec2& pos,const char* taItem,bool centered = true);
	uint32 addImageLink(float y,const char* taItem);
	DialogButton addButton(float y,const char* taItem,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	void addInputField(float x,float y,const char* regularItem,const char* highlightItem,const std::string& text,const char* cursorItem,int size);
	uint32 addText(const Vec2& pos,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(uint32 id,const std::string& text,bool centered = false);
	virtual int onButton(int button,int x,int y,bool down);
	void setButtonTexture(const DialogButton& button,const char* taItem);
	virtual void activate();
	virtual void deactivate();		
	const bool isActive() const {
		return m_Active;
	}
	const IdString& getHashName() const {
		return m_HashName;
	}
	void updateMousePos(const ds::Vec2& mousePos);
	bool OnChar(char ascii);
private:
	void setInputFieldState(uint32 id,bool selected);
	void setImageLinkTexture(uint32 id,const char* taItem);
	IdString m_HashName;
	SimpleQuadNode* m_MeshNode;
	TextNode* m_TextNode;
	ImageLinks m_ImageLinks;
	DialogButtons m_Buttons;
	InputFields m_InputFields;
	const char* m_ButtonItem;
	const char* m_ButtonItemSelected;
	bool m_Active;
	int m_Selected;
	bool m_SupportHover;
	int m_SelectedInput;
};

}
