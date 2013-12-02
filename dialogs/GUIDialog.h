#pragma once
#include "..\renderer\Renderer.h"
#include <vector>
#include "..\nodes\SpriteBatch.h"
#include "..\renderer\render_types.h"

namespace ds {

typedef uint32 DialogID;
// -------------------------------------------------------
// Dialog button
// -------------------------------------------------------
struct DialogButton {
	uint32 id;
	uint32 imageIndex;
	uint32 textIndex;
};

// -------------------------------------------------------
// Input field
// -------------------------------------------------------
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

// -------------------------------------------------------
// GUI Dialog
// -------------------------------------------------------
class GUIDialog {

struct ImageLink {
	uint32 index;
	Vec2 pos;
	Rect rect;
};

struct GUIItem {
	uint32 id;
	std::vector<Sprite> sprites;
};

typedef std::vector<ImageLink> ImageLinks;
typedef std::vector<DialogButton> DialogButtons;
typedef std::vector<InputField> InputFields;
typedef std::vector<GUIItem> Items;

public:
	GUIDialog() {}
	~GUIDialog(void);
	void init(const char* name,const DialogID& id,SpriteBatch* spriteBatch,const BitmapFont& bitmapFont);
	void render();
	void setButtonHover(const Rect& regularItem,const Rect& highlightItem);
	uint32 addImage(float y,const char* taItem);

	uint32 addImage(const Vec2& pos,const Rect& textureRect,bool centered = true);	
	uint32 addImageLink(const Vec2& pos,const Rect& textureRect,bool centered = true);
	const DialogButton& addButton(float y,const std::string& text,const Rect& textureRect,const Color& textColor = Color(1.0f,1.0f,1.0f,1.0f),float textScale = 1.0f);
	uint32 addText(const Vec2& pos,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);

	uint32 addImageLink(const Vec2& pos,const char* taItem,bool centered = true);
	uint32 addImageLink(float y,const char* taItem);
	DialogButton addButton(float y,const char* taItem,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	void addInputField(float x,float y,const char* regularItem,const char* highlightItem,const std::string& text,const char* cursorItem,int size);
	
	void updateText(uint32 id,const Vec2& pos,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	virtual int onButton(int button,int x,int y,bool down);

	void setButtonTexture(const DialogButton& button,const Rect& textureRect);
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
	const DialogID& getDialogID() const {
		return m_ID;
	}
private:
	void setInputFieldState(uint32 id,bool selected);
	void setImageLinkTexture(uint32 id,const char* taItem);
	DialogID m_ID;
	IdString m_HashName;
	SpriteBatch* m_Batch;
	BitmapFont m_BitmapFont;
	Items m_Items;
	ImageLinks m_ImageLinks;
	DialogButtons m_Buttons;
	InputFields m_InputFields;
	Rect m_ButtonItem;
	Rect m_ButtonItemSelected;
	bool m_Active;
	int m_Selected;
	bool m_SupportHover;
	int m_SelectedInput;
	uint32 m_Index;
};

}
