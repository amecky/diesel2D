#pragma once
#include "..\renderer\render_types.h"
#include "..\utils\GameTimer.h"
#include "..\io\Serializer.h"
#include "..\sprites\Sprite.h"
#include "..\base\GameObject.h"
#include "..\renderer\BitmapFont.h"

namespace ds {

const int MAX_COUNTER = 16;
const int MAX_TIMER = 8;
const int MAX_HUD_IMAGES = 64;
const int MAX_HUD_NUMBERS = 8;

class HUD : public GameObject , public Serializer {

struct NumberDef {
	float start;
	float width;
	ds::Texture texture;
};

struct HUDNumber {
	uint32 id;
	int value;
	int digits[16];
	int num;
	Vector2f positions[16];
	Texture textures[16];
	Vector2f position;
	Color color;
	int flag;
};

struct HUDEntry {
	uint32 id;
	Vector2f pos;
	ds::Color color;
	float scale;
	int flag;
	std::vector<Sprite> sprites;
};

struct HUDImage {
	uint32 id;
	int entryID;
	int flag;
};

struct HUDText {
	uint32 id;
	std::string text;
	int entryID;
	int flag;
};

struct HUDTimer {
	uint32 id;
	GameTimer gameTimer;
	int seconds;
	int flag;
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
	HUD();
	virtual ~HUD();
	void init() {}
	void init(int textureID,const char* fontName);

	void update(float elapsed);
	void render();

	void defineNumber(int index, float top, float left, float width, float height);

	int addText(int x,int y,const std::string& txt,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
			
	void addImage(int id,int x,int y,const Rect& texturRect,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	
	void addCounter(int id,int x,int y,int length,int value,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	
	void addText(int id,int x,int y,const std::string& txt,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);

	void addNumber(int id, int x, int y, int length, const ds::Color& color = ds::Color(1.0f, 1.0f, 1.0f, 1.0f), float scale = 1.0f);

	void setNumber(int id, int value);
	
	void setText(int id,const std::string& txt);
	
	void addTimer(int id,int x,int y,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	
	void setCounterValue(int id,int value);
	void setTimer(int id,int minutes,int seconds);
	void stopTimer(int id);
	void startTimer(int id);
	GameTimer* getTimer(int id);
	//! Clears all elements	 
	void clear();

	void load(BinaryLoader* loader);	

private:	
	int createEntry(const Vector2f& pos,float scale = 1.0f,const ds::Color& color = ds::Color::WHITE);
	void createText(HUDEntry* entry,const std::string& text,bool clear = true);

	int getTextIndex(int id);
	
	BitmapFont* m_Font;
	HUDText m_TextEntries[32];
	HUDImage m_Images[MAX_HUD_IMAGES];
	HUDEntry m_HUDEntries[256];
	HUDCounter m_Counter[MAX_COUNTER];
	HUDTimer m_Timer[MAX_TIMER];
	NumberDef m_Definitions[10];
	HUDNumber m_Numbers[MAX_HUD_NUMBERS];
};

}

