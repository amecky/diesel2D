#pragma once
#include "render_types.h"

namespace ds {

class Renderer;

class DebugRenderer {

struct TextLine {
	std::string message;
	Vec2 pos;
	Color color;
	TextLine() { 
		message = "" ; pos = Vec2(0,0) ; color = Color(1.0f,1.0f,1.0f,1.0f) ; 
	}
};

typedef std::vector<TextLine> TextLines;

public:
	DebugRenderer(Renderer* renderer);
	~DebugRenderer(void);
	void init();
	void clear();
	void draw();
	void showDrawCounter(const Vec2& pos);
	void showProfile(const Vec2& pos);
	void addText(const Vec2& pos,const char* text);
	void addText(const Vec2& pos,char* format,...);
	void addText(const Vec2& pos,char* format,va_list args);
	void addText(const Vec2& pos,const Color& color,char* format,...);
	void addText(const Vec2& pos,const Color& color,char* format,va_list args);	
private:
	LPD3DXSPRITE m_Sprite;
	TextLines m_TextLines;
	Renderer* m_Renderer;
	int m_Font;

};

}

