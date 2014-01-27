#pragma once
#include "..\renderer\Renderer.h"
#include <vector>
#include "..\nodes\SpriteBatch.h"
#include "..\renderer\render_types.h"
#include "..\io\Serializer.h"
#include "..\utils\PlainTextReader.h"

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
	Vec2 pos;
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
	Vec2 pos;
	Rect rect;
};



typedef std::vector<ImageLink> ImageLinks;
typedef std::vector<DialogButton> DialogButtons;
typedef std::vector<InputField> InputFields;
typedef std::vector<GUIItem> Items;

public:
	GUIDialog() {}
	~GUIDialog(void);
	void init(const char* name,const DialogID& id,SpriteBatch* spriteBatch,BitmapFont* bitmapFont);
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
	void updateMousePos(const ds::Vec2& mousePos);
	bool OnChar(char ascii);
	const DialogID& getDialogID() const {
		return m_ID;
	}
	void clear() {
		m_Items.clear();
		m_ImageLinks.clear();
		m_Buttons.clear();
	}

	//! Loads data from a json file
	/*
		\param filename the name of the json file include directories
	*/
	void load(const char* fileName);	
	//! Reloads data from a json file
	void reload(const char* fileName);
private:
	void loadDialogFromJSON(JSONReader& reader);
	GUIItem* findByID(int id);
	int findFreeID();
	DialogID m_ID;
	IdString m_HashName;
	SpriteBatch* m_Batch;
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
};

}
