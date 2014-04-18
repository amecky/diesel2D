#pragma once
#include "..\utils\SimpleQuadBuffer.h"
#include "..\content\Shader.h"
#include "..\renderer\render_types.h"
#include "gui_types.h"

namespace ds {

	class GUIButtonRenderer : public IGUICtrlRenderer {

		public:
			GUIButtonRenderer(GUICtrl* control) : IGUICtrlRenderer(control) {}
			virtual ~GUIButtonRenderer() {}
			void init(SimpleQuadBuffer* buffer);
			void update(SimpleQuadBuffer* buffer);
		private:
			uint32 m_Index;
	};
	
	/*
	class GUIControl {

typedef std::vector<GUIControl*> Children;

public:
	GUIControl(uint32 index,GUIControl* parent,SimpleQuadBuffer* buffer,const Vector2f& position) 
		: m_CtrlIndex(index) , m_Parent(parent) , m_Buffer(buffer) , m_Dimension(0,0) , m_Position(position) {}
	virtual ~GUIControl() {}
	void add(GUIControl* control) {
		m_Children.push_back(control);
	}
	const bool hasChildren() const {
		return !m_Children.empty();
	}
	const Vector2f& getDimension() const {
		return m_Dimension;
	}
	const Vector2f& getPosition() {
		return m_Position;
	}
	virtual void setColor(const Color& color) {}
	const uint32 getIndex() const {
		return m_CtrlIndex;
	}
	void find(const Vector2f& pos,GUIControl* control,int& index,bool& found);
	bool operator == (const GUIControl* ctrl);
	bool operator != (const GUIControl* ctrl);
	GUIControl* getParent() {
		return m_Parent;
	}
	virtual void onMouseOver() {
		LOG << "Control " << m_CtrlIndex << " mouse over";
	}
	virtual void onMouseOut() {
		LOG << "Control " << m_CtrlIndex << " mouse out";
	}
protected:
	bool isInside(const Vector2f& pos);
	void updatePosition(const Vector2f& pos) {
		Vector2f np = pos;
		if ( m_Parent != 0 ) {
			np += m_Parent->getPosition();
		}
		m_Position = np;
	}
	Vector2f m_Position;
	Vector2f m_Dimension;
	GUIControl* m_Parent;
	SimpleQuadBuffer* m_Buffer;
private:
	uint32 m_CtrlIndex;
	Children m_Children;
};
*/
// -------------------------------------------------------
// Window
// -------------------------------------------------------
	/*
class GUIWindow : public GUIControl {

public:
	GUIWindow(uint32 index,GUIControl* parent,SimpleQuadBuffer* buffer,const Vector2f& pos,const Vector2f& size,const Rect& textureRect) : GUIControl(index,parent,buffer,pos) {
		m_Index = buffer->create(size.x,size.y,Vector2f(0,0),textureRect,1024.0f);
		updatePosition(pos);
		buffer->update(m_Index,m_Position);
		m_Dimension = size;
	}
	virtual ~GUIWindow() {}
private:
	uint32 m_Index;
};
*/
// -------------------------------------------------------
// Image
// -------------------------------------------------------
/*
class GUIImage : public GUIControl {

public:
	GUIImage(uint32 index,GUIControl* parent,SimpleQuadBuffer* buffer,const Vector2f& pos,const Vector2f& size,const Rect& textureRect) : GUIControl(index,parent,buffer,pos) {
		m_Index = buffer->create(size.x,size.y,Vector2f(0,0),textureRect,1024.0f);
		updatePosition(pos);
		m_Dimension = size;
		buffer->update(m_Index,m_Position);
	}
	virtual ~GUIImage() {}
	void setColor(const Color& color) {
		m_Buffer->setColor(m_Index,color);
	}
private:
	uint32 m_Index;
};
*/
// -------------------------------------------------------
// Button
// -------------------------------------------------------
/*
class GUIButton : public GUIControl {

public:
	GUIButton(uint32 index,GUIControl* parent,SimpleQuadBuffer* buffer,const Vector2f& pos,const Rect& textureRect) : GUIControl(index,parent,buffer,pos) {
		m_Index = buffer->create(textureRect.width(),textureRect.height(),Vector2f(0,0),textureRect,1024.0f);
		updatePosition(pos);
		buffer->update(m_Index,m_Position);
		m_Dimension = Vector2f(textureRect.width(),textureRect.height());
	}
	virtual ~GUIButton() {}
private:
	uint32 m_Index;
};
*/
// -------------------------------------------------------
// Label
// -------------------------------------------------------
/*
class GUILabel : public GUIControl {

struct Char {
	uint32 index;
	float dimX;
	float dimY;
};

typedef std::vector<Char> Chars;

public:
	GUILabel(uint32 index,GUIControl* parent,SimpleQuadBuffer* buffer,const Vector2f& pos,const std::string& text,BitmapFont* bitmapFont) 
		: GUIControl(index,parent,buffer,pos) , m_BitmapFont(bitmapFont) , m_Text(text) {
		float padding = 0.0f;
		float scale = 1.0f;
		updatePosition(pos);		
		setText();
	}
	virtual ~GUILabel() {}
	// sets the color of all characters
	void setColor(const Color& color);
	void align();
private:
	// converts the text to quads
	void setText();
	uint32 m_Index;
	Chars m_Chars;
	BitmapFont* m_BitmapFont;
	std::string m_Text;
};
*/
// -------------------------------------------------------
// 
// -------------------------------------------------------
struct GUIData {
	
};

class GUINode : public Node {

public:
	GUINode(const char* name,const char* material,const char* bitmapFont);
	virtual ~GUINode(void) {}
	// creates a new button
	//GUIButton* createButton(GUIControl* parent,const Vector2f& pos,const Rect& textureRect);
	// creates new window
	//GUIWindow* createWindow(GUIControl* parent,const Vector2f& pos,const Vector2f& size,const Rect& textureRect);
	GCID createImage(GCID parent,const Vector2f& pos,const Vector2f& size,const Rect& textureRect);
	// creates new image
	//GUIImage* createImage(GUIControl* parent,const Vector2f& pos,const Vector2f& size,const Rect& textureRect);
	// creates new label
	//GUILabel* createLabel(GUIControl* parent,const Vector2f& pos,const std::string& text);
	// update
	void update(float elapsed);
	// these methods just redirect directly to the buffer
	void init() {
		m_Buffer->init();
	}
	void prepareRendering() {
		m_Buffer->prepareRendering();
	}
	void postRendering() {
		m_Buffer->postRendering();
	}
	void draw() {
		m_Buffer->draw();
	}
private:
	//void addControl(GUIControl* control);
	uint32 m_Counter;
	GUICtrl* m_MainControl;
	GUICtrl* m_Selected;
	//int m_SelectedIndex;
	SimpleQuadBuffer* m_Buffer;
	BitmapFont* m_Font;
};

}

