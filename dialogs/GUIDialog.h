#pragma once
#include <vector>
#include "..\renderer\render_types.h"
#include "..\io\Serializer.h"
#include "..\sprites\Sprite.h"
#include "..\math\tweening.h"
#include "..\renderer\BitmapFont.h"
#include "..\ui\IMGUI.h"
#include "..\utils\GameTimer.h"
#include "..\compiler\DataFile.h"

namespace ds {

typedef uint32 DialogID;
const int MAX_GUID = 64;

// -------------------------------------------------------
// GUI item types
// -------------------------------------------------------
enum GUIItemType {
	GIT_BUTTON,
	GIT_TEXT,
	GIT_IMAGE,
	GIT_IMAGE_BUTTON,
	GIT_TIMER,
	GIT_NUMBERS,
	GIT_UNUSED
};

// -------------------------------------------------------
// GUID
// -------------------------------------------------------
struct GUID {
	int id;
	int index;
	int entryIndex;

	GUID() : id(-1), index(-1) , entryIndex(-1) {}
	GUID(int _id, int _index, int _entryIndex) : id(_id), index(_index) , entryIndex(_entryIndex) {}
};

struct GUINumber {
	int value;
	int length;
};

struct GUIText {
	char text[32];
	v2 size;
};

struct GUIButton {
	char text[32];
	v2 size;
	Texture texture;
	Rect boundingRect;
};

struct GUIImageButton {
	v2 size;
	Texture texture;
	Rect boundingRect;
};

struct GUIImage {
	Texture texture;
};

// -------------------------------------------------------
// GUIItem
// -------------------------------------------------------
struct GUIItem {
	GUID gid;
	uint32 id;
	v2 pos;
	bool centered;
	Color color;
	float scale;
	GUIItemType type;
	
	v2 size;

	GUIItem() : id(-1) , pos(0,0) , centered(true) , color(Color::WHITE) , scale(1.0f) , size(0,0) {}
};

// -------------------------------------------------------
// GUI model item
// -------------------------------------------------------
struct GUIModelItem {
	int id;
	GUIItemType type;
};

// -------------------------------------------------------
// GUI transitions
// -------------------------------------------------------
struct GUITransition {
	int id;
	int typeBits;
	float timer;
	float ttl;
};

// -------------------------------------------------------
// GUI Dialog
// -------------------------------------------------------
class GUIDialog : public DataFile {

typedef std::vector<GUIItem> Items;
typedef std::vector<GUINumber> Numbers;
typedef std::vector<GUIText> Texts;
typedef std::vector<GUIImage> Images;
typedef std::vector<GUIButton> Buttons;
typedef std::vector<GUIImageButton> ImageButtons;
typedef std::vector<GameTimer> Timers;

public:
	GUIDialog();
	~GUIDialog(void);
	void init(const char* name,const DialogID& id,BitmapFont* bitmapFont);
	void render();
	void setButtonHover(const Rect& regularItem,const Rect& highlightItem);
	

	GUID addImage(int id,int x,int y,const Rect& textureRect,float scale = 1.0f,bool centered = true);	
	void updateImage(int id, int x, int y, const Rect& textureRect, bool centered = true);

	GUID addImageButton(int id,int x,int y,const Rect& textureRect,bool centered = true);

	// FIXME: add button with x and y position
	GUID addButton(int id,float x,float y,const char* text,const Rect& textureRect,const Color& textColor = Color(1.0f,1.0f,1.0f,1.0f),float textScale = 1.0f,bool centered = true);
	void setButtonTexture(int id,const Rect& textureRect);

	GUID addText(int id,int x,int y,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(int id,int x,int y,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(int id,const std::string& text);
	v2 getTextSize(int id);

	GUID addTimer(int id,int x, int y, float scale = 1.0f, const Color& color = Color::WHITE, bool centered = true);
	void resetTimer(int id);
	void startTimer(int id);
	GameTimer* getTimer(int id);

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
	const DialogID& getDialogID() const {
		return m_ID;
	}
	void clear();

	void tick(float dt);
	
	void showDialog();

	GUID addNumber(int id,const v2& position,int value, int length,float scale = 1.0f,const Color& color = Color::WHITE,bool centered = false);
	void setNumber(int id, int value);

	void setTransition(int id, int type, float ttl);

	bool saveData(JSONWriter& writer);
	bool loadData(JSONReader& reader);
	const char* getFileName() const {
		return _jsonName;
	}
	const char* getName() const {
		return _name;
	}
private:
	GUIDialog(const GUIDialog& other) {}
	void saveItem(BinaryWriter& writer, int id, const GUIItem& item);
	void saveItem(JSONWriter& writer, int id, const GUIItem& item);
	int loadItem(BinaryLoader& loader, GUIItem* item);
	int loadItem(Category* category, GUIItem* item);
	void addToModel(int id, GUIItemType type,const char* prefix);
	void showAddDialog();
	GUIItem* findByID(int id);
	int findFreeID();
	bool containsItem(int id);
	int getNextID();
	bool swap(int currentIndex, int newIndex);
	int createItem(const v2& position, GUIItemType type, float scale = 1.0f, bool centered = true, const Color& color = Color::WHITE);
	int getIndexByID(int id);
	bool remove(int id);
	v2 getPosition(int index);

	DialogID m_ID;
	IdString m_HashName;
	BitmapFont* m_BitmapFont;
	Items m_Items;
	Rect m_ButtonItem;
	Rect m_ButtonItemSelected;
	bool m_Active;
	int m_Selected;
	bool m_SupportHover;
	int m_SelectedInput;

	std::vector<std::string> _availableElements;
	int _selectedElement;
	int _elementOffset;
	gui::ComponentModel<GUIModelItem> _model;
	int _state;
	int _offset;
	v2 _position;
	bool _showAdd;
	char _name[32];
	char _jsonName[128];

	GUID _ids[MAX_GUID];
	int _idIndex;
	GUITransition _transitions[MAX_GUID];	
	int _transitionCounter;
	bool _transitionMode;

	Numbers _numbers;
	Texts _texts;
	Images _images;
	Buttons _buttons;
	ImageButtons _imageButtons;
	Timers _timers;
};

}
