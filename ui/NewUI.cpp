#include "NewUI.h"

//const int UPPERCASE[] = {0,12,14,23,25,36,39,50,53,61,64,72,73,85,88,98,101,107,108,114,118,128};
const int UPPERCASE[] = {0,9, 10,17, 19,26, 28,36, 38,44, 46,52, 54,62, 64,72, 74,78, 79,85, 87,94, 96,103, 104,113, 115,123, 125,134, 136,144, 145,154, 156,164, 165,172, 173,181, 182,190, 191,200, 200,214, 214,223, 223,233, 234,240};
//const int LOWERCASE[] = {0,9, 12,21, 22,30, 31,40, 42,52, 53,59, 59,68, 71,79, 82,84, 85,90, 93,103, 104,106, 110,124, 127,135};
const int LOWERCASE[] = {0,8, 8,15, 17,23, 24,30, 32,38, 39,44, 45,51, 53,59, 61,63, 64,68, 70,77, 78,80, 82,92, 93,100, 102,108, 110,116, 118,124, 126,131, 132,137, 138,143, 144,150, 151,159, 159,171, 171,179, 179,187, 188,193};
const int SPECIAL[] = {0,4, 6,11, 13,21, 23,30, 32,45, 47,55, 57,59, 60,66, 68,74, 76,81, 84,91, 93,98, 98,102, 104,106, 107,115, 116,123, 126,132, 134,141, 143,150, 152,159, 161,168, 170,177, 179,186, 188,195, 197,204, 206,208, 210,214, 216,223, 226,233, 236,243, 246,251};
namespace ds {

// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
NewUIText::NewUIText(uint32 id,const Vec2& pos,const std::string& txt) : UIWidget(id) , m_Position(pos) {
	m_Text = txt;
	for ( uint32 i = 0; i < 256;++i) {
		m_CharMapping[i].supported = false;
	}
	int o = 0;
	for ( uint32 i = 97; i < 123;++i ) {
		int left = LOWERCASE[o];
		int width = LOWERCASE[o+1] - LOWERCASE[o];
		m_CharMapping[i].textureRect = Rect(340,left,width,17);
		m_CharMapping[i].supported = true;
		m_CharMapping[i].charName = i;
		o += 2;
	}
	o = 0;
	for ( uint32 i = 65; i < 91;++i ) {
		m_CharMapping[i].textureRect = Rect(358,UPPERCASE[o],UPPERCASE[o+1]-UPPERCASE[o],13);
		m_CharMapping[i].supported = true;
		m_CharMapping[i].charName = i;
		o += 2;
	}
	o = 0;
	for ( uint32 i = 33; i < 64;++i ) {
		m_CharMapping[i].textureRect = Rect(373,SPECIAL[o],SPECIAL[o+1]-SPECIAL[o],14);
		m_CharMapping[i].supported = true;
		m_CharMapping[i].charName = i;
		o += 2;
	}
}

void NewUIText::update(float elapsed) {

}

void NewUIText::fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad) {
	Vec2 absPos = parentPosition;
	absPos += m_Position;
	uint32 length = m_Text.length();
	for ( uint32 i = 0; i < length; ++i ) {
		char c = m_Text[i];
		if ( c == ' ' ) {
			absPos.x += 14.0f;
		}
		else {
			Character* ch = &m_CharMapping[c];
			if ( ch->supported ) {			
				screenQuad.add(absPos.x,absPos.y,ch->textureRect.width(),ch->textureRect.height(),ch->textureRect);
				absPos.x += (ch->textureRect.width()+3.0f);
			}		
		}
	}
}

// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
NewUILabel::NewUILabel(uint32 id,const Vec2& pos,const std::string& text) : UIWidget(id) , m_Position(pos) {
	m_Text = new NewUIText(id,pos,text);
}

void NewUILabel::update(float elapsed) {

}

void NewUILabel::fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad) {
	Vec2 absPos = parentPosition;
	absPos += m_Position;
	m_Text->fillBuffer(absPos,screenQuad);
}

// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
NewUITextbox::NewUITextbox(uint32 id,const Vec2& pos,uint32 characters) : UIWidget(id) , m_Position(pos) {
	m_Size = CHARACTER_WIDTH * characters;
	m_Rect = Rect(0,260,m_Size,25);
}

void NewUITextbox::update(float elapsed) {

}

void NewUITextbox::fillBuffer(const Vec2& parentPosition,ScreenQuad& screenQuad) {
	Vec2 absPos = parentPosition;
	absPos += m_Position;
	screenQuad.add(absPos.x,absPos.y,m_Size,25.0f,m_Rect);
}

// ---------------------------------------------------------------
//
// ---------------------------------------------------------------
NewUIDialog::NewUIDialog(const char* name,const char* materialName)
	: ScreenQuad(name,materialName,512,true) {
	m_Position = Vec2(10,10);
	m_Size = Vec2(200,200);
	m_Rect = Rect(0,0,256,256);
}

void NewUIDialog::update(float elapsed) {
	clear();
	add(m_Position.x,m_Position.y,m_Size.x,m_Size.y,m_Rect);
	for ( size_t i = 0; i < m_Widgets.num(); ++i ) {
		UIWidget* widget = m_Widgets[i];
		widget->fillBuffer(m_Position,*this);	
	}
	rebuild();
}

void NewUIDialog::addTextbox(const Vec2& pos) {
	m_Widgets.append(new NewUITextbox(1,pos));
}

void NewUIDialog::addLabel(uint32 id,const Vec2& pos,const std::string& text) {
	m_Widgets.append(new NewUILabel(id,pos,text));
}

}
