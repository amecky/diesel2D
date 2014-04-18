#include "GUINode.h"

namespace ds {
/*
// -------------------------------------------------------
// GUIControl operator !=
// -------------------------------------------------------
bool GUIControl::operator !=(const GUIControl* ctrl) {
	return m_CtrlIndex != ctrl->getIndex();
}

// -------------------------------------------------------
// GUIControl operator ==
// -------------------------------------------------------
bool GUIControl::operator ==(const GUIControl* ctrl) {
	return m_CtrlIndex == ctrl->getIndex();
}

// -------------------------------------------------------
// GUIControl - isInside
// -------------------------------------------------------
bool GUIControl::isInside(const Vector2f& pos) {
	if ( pos.x < m_Position.x ) {
		return false;
	}
	if ( pos.y < m_Position.y ) {
		return false;
	}
	if ( pos.x > ( m_Position.x + m_Dimension.x ) ) {
		return false;
	}
	if ( pos.y > ( m_Position.y + m_Dimension.y ) ) {
		return false;
	}
	return true;	
}

// -------------------------------------------------------
// GUIControl - find
// -------------------------------------------------------
void GUIControl::find(const Vector2f& pos,GUIControl* control,int& index,bool& found) {
	if ( hasChildren() ) {
		for ( std::size_t i = 0; i < m_Children.size(); ++i ) {
			m_Children[i]->find(pos,control,index,found);			
		}
	}
	else {
		if ( isInside(pos) ) {
			found = true;
			index = getIndex();
			control = this;
		}
	}
	if ( !found ) {
		if ( isInside(pos) ) {
			found = true;
			index = getIndex();
			control = this;
		}
	}     
}

// -------------------------------------------------------
// GUILabel set color
// -------------------------------------------------------
void GUILabel::setColor(const Color& color) {
	for ( size_t i = 0 ; i < m_Chars.size(); ++i ) {
		Char* c = &m_Chars[i];
		m_Buffer->setColor(c->index,color);
	}
}

void GUILabel::setText() {
	
	float padding = 0.0f;
	float scale = 1.0f;
	float x = m_Position.x;
	float y = m_Position.y;
	float texSize = (float)m_BitmapFont->getTextureSize();
	float z = 0;
	float dim = 0.0f;
	for ( size_t cnt = 0; cnt < m_Text.length(); ++cnt ) {
		char c = m_Text[cnt];
		CharDef cd = m_BitmapFont->getCharDef(c);
		x += padding;
		padding = (cd.width+2)  * scale;
		float dimX = cd.width * scale;
		dim += dimX;
		float dimY = m_BitmapFont->getCharHeight() * scale;
		Char ch;
		ch.index = m_Buffer->create(dimX,dimY,m_Position,Rect(0,0,0,0),1024.0f);
		ch.dimX = padding;
		ch.dimY = dimY;
		LOG << "char dim " << ch.dimX << " / " << ch.dimY;
		m_Buffer->setTextureCoordinates(ch.index,cd.u1,cd.v1,cd.u2,cd.v2);
		m_Buffer->update(ch.index,Vector2f(x,y));
		m_Chars.push_back(ch);				
	}
	float height = m_BitmapFont->getCharHeight() * scale;
	m_Dimension = Vector2f(dim,height);
}

void GUILabel::align() {
	if ( m_Parent != 0 ) {
		Vector2f parentPos = m_Parent->getPosition();
		Vector2f parentDim = m_Parent->getDimension();
		float cx = (parentDim.x - m_Dimension.x) * 0.5f;
		float cy = (parentDim.y - m_Dimension.y) * 0.5f;
		m_Position = parentPos + Vector2f(cx,cy);
		Vector2f p = m_Position;
		for ( size_t i = 0; i < m_Chars.size();++i ) {
			Char* c = &m_Chars[i];
			m_Buffer->update(c->index,Vector2f(p.x,p.y));
			p.x += c->dimX;
		}
	}
}
*/
// -------------------------------------------------------
// GUINode
// -------------------------------------------------------
GUINode::GUINode(const char* name,const char* material,const char* bitmapFont) : Node(name,material,true) , m_Counter(0) , m_Selected(0) , m_MainControl(0)  {
	m_Buffer = new SimpleQuadBuffer(1024);
	/*
	Material* mtrl = gEngine->getResourceManager().getMaterialByName(material);
	if ( mtrl != 0 ) {
		LOG << "material: " << mtrl->getTexture(0)->getName();
		ResourceHandle mh = gEngine->getResourceManager().findByName(material,RS_MATERIAL);
		if ( gEngine->getResourceManager().contains("GUIShader",RS_SHADER) ) {
			ResourceHandle sh = gEngine->getResourceManager().findByName("GUIShader",RS_SHADER);
			gEngine->getResourceManager().assignShader(sh,mh);
		}
		else {
			TransformedTextureColorShader* shader = new TransformedTextureColorShader("GUIShader",mtrl->getTexture(0)->getName());
			ds::ResourceHandle sh = gEngine->getResourceManager().addShader("GUIShader",shader);
			gEngine->getResourceManager().assignShader(sh,mh);
		}
	}		
	*/
	m_Font = gEngine->getResourceManager().getBitmapFontByName(bitmapFont);
}

// -------------------------------------------------------
// internal add control
// -------------------------------------------------------
/*
void GUINode::addControl(GUIControl* control) {
	++m_Counter;
	if ( m_MainControl == 0 ) {
		m_MainControl = control;
	}
	if ( control->getParent() != 0 ) {
		control->getParent()->add(control);
	}
}
// creates a new button
GUIButton* GUINode::createButton(GUIControl* parent,const Vector2f& pos,const Rect& textureRect) {
	GUIButton* button = new GUIButton(m_Counter,parent,m_Buffer,pos,textureRect);
	addControl(button);
	return button;
}
// create new window
GUIWindow* GUINode::createWindow(GUIControl* parent,const Vector2f& pos,const Vector2f& size,const Rect& textureRect) {
	GUIWindow* window = new GUIWindow(m_Counter,parent,m_Buffer,pos,size,textureRect);
	addControl(window);
	return window;
}
// create new image
GUIImage* GUINode::createImage(GUIControl* parent,const Vector2f& pos,const Vector2f& size,const Rect& textureRect) {
	GUIImage* image = new GUIImage(m_Counter,parent,m_Buffer,pos,size,textureRect);
	addControl(image);
	return image;
}

GUILabel* GUINode::createLabel(GUIControl* parent,const Vector2f& pos,const std::string& text) {
	GUILabel* label = new GUILabel(m_Counter,parent,m_Buffer,pos,text,m_Font);
	addControl(label);
	return label;
}
*/

GCID GUINode::createImage(GCID parent,const Vector2f& pos,const Vector2f& size,const Rect& textureRect) {
	GUICtrl* ctrl = new GUICtrl();
	if ( parent != GC_NONE ) {
		ctrl->parent = parent;
	}
	ctrl->position = pos;
	ctrl->dimension = size;
	ctrl->index = m_Counter;
	ctrl->next = 0;
	++m_Counter;    
	/*
	if ( parent != GC_NONE && m_MainControl != 0 ) {        
		GUICtrl* parentCtrl = gui::findByID(m_MainControl,parent);
		if ( parentCtrl->next == 0 ) {
			parentCtrl->next = ctrl;
		}
		else {
			GUICtrl* current = parentCtrl->next;
			while ( current != 0 ) {
				current = current->next;
			}
			current->next = ctrl;
		}
	}    
	if ( m_MainControl == 0 ) {
		m_MainControl = ctrl;
	}
	*/
	return ctrl->index;
}

void GUINode::update(float elapsed) {
	Vector2f mousePos = gEngine->getMousePosition();
	if ( m_MainControl != 0 ) {
		/*
		bool found = false;
		int idx = -1;
		GUIControl* tmp = m_MainControl;
		m_MainControl->find(mousePos,tmp,idx,found);
		if ( found ) {
			//GUIControl* tmp = m_MainControl->findById(idx);
			if ( tmp != 0 ) {
				if ( m_Selected != 0 && m_Selected != tmp ) {
					m_Selected->onMouseOut();
					m_Selected = tmp;
					m_Selected->onMouseOver();
				}
				else if ( m_Selected == 0 ) {
					m_Selected = tmp;
					m_Selected->onMouseOver();
				}
			}
			else {
				LOG << "Found but cannot find control";
			}
		}
		*/
	}
}

}
