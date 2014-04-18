#pragma once
#include "..\renderer\Renderer.h"
#include <vector>
#include "..\renderer\render_types.h"
#include "..\io\Serializer.h"
#include "..\sprites\Sprite.h"

namespace ds {

typedef uint32 DialogID;


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

struct GUIItem {
	uint32 id;
	Vector2f pos;
	bool centered;
	Color color;
	float scale;
	std::vector<Sprite> sprites;

	GUIItem() : id(-1) , pos(0,0) , centered(true) , color(Color::WHITE) , scale(1.0f) {}
};
// -------------------------------------------------------
// GUI Dialog
// -------------------------------------------------------
class GUIDialog : public Serializer {

struct DialogButton {
	uint32 id;
	uint32 imageIndex;
	uint32 textIndex;
};

struct ImageLink {
	uint32 index;
	Vector2f pos;
	Rect rect;
};



typedef std::vector<ImageLink> ImageLinks;
typedef std::vector<DialogButton> DialogButtons;
typedef std::vector<InputField> InputFields;
typedef std::vector<GUIItem> Items;

public:
	GUIDialog(int screenWidth,int screenHeight) : m_ScreenWidth(screenWidth) , m_ScreenHeight(screenHeight) {}
	~GUIDialog(void);
	void init(const char* name,const DialogID& id,Renderer* renderer,BitmapFont* bitmapFont);
	void render();
	void setButtonHover(const Rect& regularItem,const Rect& highlightItem);
	

	uint32 addImage(int x,int y,const Rect& textureRect,bool centered = true);	
	uint32 addImageLink(int x,int y,const Rect& textureRect,bool centered = true);

	// FIXME: add button with x and y position
	void addButton(int id,float y,const std::string& text,const Rect& textureRect,const Color& textColor = Color(1.0f,1.0f,1.0f,1.0f),float textScale = 1.0f);
	void setButtonTexture(int id,const Rect& textureRect);

	void addText(int id,int x,int y,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(int id,int x,int y,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(int id,const std::string& text);

	uint32 addImageLink(int x,int y,const char* taItem,bool centered = true);
	
	virtual int onButton(int button,int x,int y,bool down);

	virtual void activate();
	virtual void deactivate();		
	const bool isActive() const {
		return m_Active;
	}
	const IdString& getHashName() const {
		return m_HashName;
	}
	void updateMousePos(const Vector2f& mousePos);
	bool OnChar(char ascii);
	const DialogID& getDialogID() const {
		return m_ID;
	}
	void clear() {
		m_Items.clear();
		m_ImageLinks.clear();
		m_Buttons.clear();
	}
	void load(BinaryLoader* loader);
private:
	GUIItem* findByID(int id);
	int findFreeID();
	DialogID m_ID;
	IdString m_HashName;
	Renderer* m_Renderer;
	BitmapFont* m_BitmapFont;
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
	int m_ScreenWidth;
	int m_ScreenHeight;
};

}
