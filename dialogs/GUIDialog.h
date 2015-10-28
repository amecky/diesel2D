#pragma once
#include <vector>
#include "..\renderer\render_types.h"
#include "..\io\Serializer.h"
#include "..\sprites\Sprite.h"
#include "..\math\tweening.h"
#include "..\renderer\BitmapFont.h"
#include "..\ui\IMGUI.h"

namespace ds {

typedef uint32 DialogID;

enum GUIItemType {
	GIT_BUTTON,
	GIT_TEXT,
	GIT_IMAGE,
	GIT_IMAGELINK
};

struct GUIItem {
	uint32 id;
	v2 pos;
	bool centered;
	Color color;
	float scale;
	GUIItemType type;
	Rect boundingRect;
	char label[32];
	int textSize;
	v2 size;
	Texture texture;

	GUIItem() : id(-1) , pos(0,0) , centered(true) , color(Color::WHITE) , scale(1.0f) , size(0,0) {}
};

struct GUIModelItem {
	uint32 id;
	GUIItemType type;
};

// -------------------------------------------------------
// GUI base effect
// -------------------------------------------------------
class GUIEffect {

public:
	GUIEffect() {}
	virtual ~GUIEffect() {}
	void init(GUIItem* guiItem) {
		item = guiItem;
	}
	virtual bool tick(float dt) = 0;
	const bool isActive() const {
		return m_Active;
	}
	void setActive(bool act) {
		if (act) {
			m_Timer = 0.0f;
		}
		m_Active = act;
	}
protected:
	bool m_Active;
	float m_Timer;
	float m_TTL;
	GUIItem* item;
};

// -------------------------------------------------------
// GUI transition effect
// -------------------------------------------------------
class TransitionEffect : public GUIEffect {

public:
	TransitionEffect(const Vector2f& start,const Vector2f& end,float ttl) : GUIEffect() {
		m_TTL = ttl;
		startPos = start;
		endPos = end;
	}
	virtual ~TransitionEffect() {}
	bool tick(float dt) {
		if (isActive()) {
			m_Timer += dt;
			if (m_Timer >= m_TTL) {
				item->pos = tweening::interpolate(tweening::easeOutQuad, startPos, endPos, 1.0f);
				setActive(false);
				return true;

			}
			float norm = m_Timer / m_TTL;
			item->pos = tweening::interpolate(tweening::easeOutQuad,startPos, endPos, norm);
		}
		return false;
	}
private:
	Vector2f startPos;
	Vector2f endPos;
};

// -------------------------------------------------------
// GUI Dialog
// -------------------------------------------------------
class GUIDialog : public Serializer {

typedef std::vector<GUIItem> Items;
typedef std::vector<GUIEffect*> Effects;

public:
	GUIDialog();
	~GUIDialog(void);
	void init(const char* name,const DialogID& id,BitmapFont* bitmapFont);
	void render();
	void setButtonHover(const Rect& regularItem,const Rect& highlightItem);
	

	uint32 addImage(int id,int x,int y,const Rect& textureRect,bool centered = true);	
	void updateImage(int id, int x, int y, const Rect& textureRect, bool centered = true);

	uint32 addImageLink(int id,int x,int y,const Rect& textureRect,bool centered = true);

	// FIXME: add button with x and y position
	void addButton(int id,float x,float y,const char* text,const Rect& textureRect,const Color& textColor = Color(1.0f,1.0f,1.0f,1.0f),float textScale = 1.0f,bool centered = true);
	void setButtonTexture(int id,const Rect& textureRect);

	void addText(int id,int x,int y,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(int id,int x,int y,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f,bool centered = true);
	void updateText(int id,const std::string& text);
	v2 getTextSize(int id);

	uint32 addImageLink(int id,int x,int y,const char* taItem,bool centered = true);
	
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
	void clear() {
		m_Items.clear();
	}
	void load(BinaryLoader* loader);

	void tick(float dt);

	void addTransition(int id, Vector2f& start,float ttl) {
		GUIItem* item = findByID(id);
		if (item != 0) {
			TransitionEffect* t = new TransitionEffect(start,item->pos,ttl);
			t->init(item);
			m_Effects.push_back(t);
		}
	}
	void showDialog();
	
private:
	void save();
	void load();
	void addToModel(int id, GUIItemType type,const char* prefix);
	void showAddDialog();
	GUIItem* findByID(int id);
	int findFreeID();
	bool containsItem(int id);
	int getNextID();
	bool swap(int currentIndex, int newIndex);

	DialogID m_ID;
	IdString m_HashName;
	BitmapFont* m_BitmapFont;
	Items m_Items;
	Effects m_Effects;
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

	
};

}
