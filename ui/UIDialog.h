#pragma once
#include "..\nodes\PlainTextNode.h"
#include "..\lib\container\List.h"
#include "..\nodes\GroupNode.h"
#include "..\nodes\IVBufferNode.h"

namespace ds {

typedef IVBufferNode<TransformedTextureColorVertex,TransformedTextureColorBuffer> UIBuffer;

enum UIControlAlignment {
	UI_AL_LEFT,
	UI_AL_CENTER,
	UI_AL_RIGHT
};

enum UIControlType {
	UI_CONTROL_BUTTON,
	UI_CONTROL_LABEL,
	UI_CONTROL_CHECKBOX,
	UI_CONTROL_RADIOBUTTON,    
	UI_CONTROL_INPUT,
	UI_CONTROL_IMAGE,
	UI_CONTROL_IMAGE_BUTTON,
	UI_CONTROL_TEXTBOX,
	UI_DROP_DOWN
};

enum UI_EVENT_TYPE {
	UI_EVENT_BUTTON_CLICKED
};


// =======================================================
// 
// =======================================================
class IUIEvent {

public:
	virtual const int getEventType() const = 0;
	virtual const uint32 getControlID() const = 0;
};

// =======================================================
// 
// =======================================================
class BaseUIEvent : public IUIEvent {

public:
	BaseUIEvent(int type,uint32 id) : m_Type(type) , m_ID(id) {}
	~BaseUIEvent() {}
	const int getEventType() const { return m_Type; }
	const uint32 getControlID() const { 
		return m_ID; 
	}
private:
	uint32 m_ID;
	int m_Type;
};

// =======================================================
// 
// =======================================================
class IUIEventListener {

public:
	virtual void onUIEvent(const IUIEvent& event) = 0;
};

class UIDialog;
// =======================================================
// The base implementation of a UI control
// =======================================================
class UIControl {

public:
	UIControl(UIDialog *dialog,uint32 ID) : m_DiaLOGC(dialog) , m_ID(ID) {}
	virtual ~UIControl(void) {};
	const uint32 getID() const { 
		return m_ID; 
	}
	virtual void onFocus() {}
	virtual void onStateChanged(bool active) {}
	virtual bool isSelected(const Vector2f& mousePos) { 
		return false; 
	}
	virtual void onChar(char ascii) {}	
	virtual const UIControlType getType() const = 0;
	virtual void fillBuffer(const Vector2f& basePosition,UIBuffer& buffer) = 0;
	virtual Vector2f& getExtent() = 0;
	Vector2f& getPosition() {
		return m_Position;
	}
	virtual void align(const Vector2f& gridPosition) = 0;
	virtual void update(float elapsed) {}
protected:
	Vector2f m_Position;
	UIDialog* m_Dialog;
	uint32 m_ID;
	
};

class UILabel;
class UIInput;
class UIButton;
class UITextbox;
class UICheckBox;
class UIDropDown;
// =======================================================
// 
// =======================================================
class UIDialog  : public Node {

struct GridCell {
	int gx;
	int gy;
	uint32 controlID;
	bool used;
	Vector2f cellPos;
	Vector2f cellSize;
	UIControlAlignment alignment;
};
	 


typedef List<GridCell> Cells;
typedef List<UIControl*> Items;
typedef List<IUIEventListener*> Listeners;

public:
	UIDiaLOGC(const char* dialogName,const char* materialName,const char* bitmapFont,const char* layerName,const Vector2f& pos,int gridSizeX,int gridSizeY);
	~UIDiaLOGC(void);
	void update(float elapsedTime);
	// mouse controller
	void OnButtonDown(int button) {}
	void OnButtonUp(int button);
	// keyboard controller
	void OnKeyUp( WPARAM virtualKeyCode ) {};
	void OnKeyDown( WPARAM virtualKeyCode ) {};
	void OnChar(char ascii,unsigned int keyState);	
	void activate();
	void deactivate();
	const bool isActive();
	bool containsControl(uint32 id);
	Vector2f& resize(float minSize = -1.0f);
	// handling controls
	void addLabel(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,UIControlAlignment alignment = UI_AL_LEFT);
	void addButton(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,UIControlAlignment alignment = UI_AL_LEFT);
	void addTextbox(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,const std::string& value,UIControlAlignment alignment = UI_AL_LEFT);
	void addCheckbox(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,bool value,UIControlAlignment alignment = UI_AL_LEFT);	
	void addDropDown(uint32 id,uint32 gridX,uint32 gridY,const std::string& label,UIControlAlignment alignment = UI_AL_LEFT);	
	// getting controls
	UICheckBox* getCheckbox(uint32 id);
	UILabel* getLabel(uint32 id);
	UIButton* getButton(uint32 id);
	UITextbox* getTextbox(uint32 id);
	UIDropDown* getDropDown(uint32 id);
	// basic methods
	void setPosition(int x,int y);
	
	// event handling
	void registerListener(IUIEventListener* listener);
	void sendEvent(const IUIEvent& event);
	Vector2f getAbsPos(const Vector2f& relPos);
	void setMouseOffset(const Vector2f& mo) {
		m_MouseOffset = mo;
	}
	void setHeader(const char* header);
	BitmapFont* getBitmapFont() {
		return m_Font;
	}
private:
	//
	UIControl* getControl(uint32 id,UIControlType type);
	UIControl* getControl(uint32 id);
	void setState(bool active);
	void setCellOwner(uint32 id,int gx,int gy,UIControlAlignment alignment);
	//
	std::string m_DialogName;
	BitmapFont* m_Font;
	IVBufferNode<TransformedTextureColorVertex,TransformedTextureColorBuffer>* m_BufferNode;
	Items m_Items;
	UIControl *m_Selected;
	bool m_Active;
	Listeners m_Listeners;
	Vector2f m_BasePos;
	Vector2f m_MouseOffset;
	Rect m_Rect;
	Vector2f m_Size;
	Cells m_Cells;
	uint32 m_GridSizeX;
	uint32 m_GridSizeY;
	bool m_ShowHeader;	
	Vector2f m_HeaderSize;
	float m_MinSize;
};

}

