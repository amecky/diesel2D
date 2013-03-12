#pragma once
#include "..\utils\QuadBuffer.h"
#include <vector>
#include <string>
// -------------------------------------------------------------
//  BaseHUD
// -------------------------------------------------------------

namespace ds {

class BaseHUD {

struct HUDEntry {
	uint32 id;
	ds::Vec2 pos;
	ds::Color color;
	float scale;
	int flag;
	std::vector<int> vertices;
};

struct HUDText {
	uint32 id;
	std::string text;
	int entryID;
	int flag;
};

struct HUDTimer {
	uint32 id;
	int seconds;
	float timer;
	int minutes;
	int flag;
	int mode;
	bool forward;
	int entryID;
};

struct HUDCounter {
	uint32 id;		
	int value;
	int length;
	int entryID;
	int flag;
};

public:
	BaseHUD(ds::Renderer* renderer,int material,int bitmapFont);
	virtual ~BaseHUD() {
		delete m_Buffer;
	}	
	void render();
	void addImage(const ds::Vec2& pos,const ds::Rect& textureRect,float textureSize = 1024.0f);
	int addText(const ds::Vec2& pos,const std::string& txt,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	int addCounter(const ds::Vec2& pos,int length,int value,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	int addTimer(const ds::Vec2& pos,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	void setCounterValue(int id,int value);
	void setTimer(int id,int minutes,int seconds);
	void updateText(int id,const std::string& text);
	void tick(float elapsed);
private:	
	int createEntry(const ds::Vec2& pos,float scale = 1.0f,const ds::Color& color = ds::Color::WHITE);
	void createText(HUDEntry* entry,const std::string& text,bool clear = true);
	ds::Renderer* m_Renderer;
	int m_Mtrl;
	int m_BitmapFont;
	ds::QuadBuffer* m_Buffer;
	HUDText m_TextEntries[32];
	HUDEntry m_HUDEntries[128];
	HUDCounter m_Counter[16];
	HUDTimer m_Timer[16];
	// basic
	bool m_Running;
	int m_ScoreIdx;
	int m_UCIdx;
	int m_LvlIdx;
	int m_HealthIdx;
};

}
