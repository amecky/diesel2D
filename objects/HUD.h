#pragma once
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"
#include "..\utils\GameTimer.h"
#include "..\io\Serializer.h"
#include "..\sprites\Sprite.h"
#include "..\base\GameObject.h"

namespace ds {

const int MAX_COUNTER = 16;
const int MAX_TIMER = 8;
const int MAX_HUD_IMAGES = 64;

class HUD : public GameObject , public Serializer {

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
	int addText(int x,int y,const std::string& txt,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
			
	void addImage(int id,int x,int y,const Rect& texturRect,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	/*
	 * Add new counter
	 *
	 * @param id - the id of the counter
	 * @param x - x position
	 * @param y - y position
	 * @param length - number of digits
	 * @param value - initial value
	 * @param color - font color
	 * @param scale - scale of the text
	 */
	void addCounter(int id,int x,int y,int length,int value,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	/*
	 * Add new text
	 *
	 * @param id - the id of the counter
	 * @param x - x position
	 * @param y - y position
	 * @param text - the text itself
	 * @param color - font color
	 * @param scale - scale of the text
	 */
	void addText(int id,int x,int y,const std::string& txt,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	/*
	 * Set new text. Only the text will be updated. Position and everything else stays the same
	 *
	 * @param id - the id of the text
	 * @param txt - the new text
	 */
	void setText(int id,const std::string& txt);
	/*
	 * Add new counter
	 *
	 * @param id - the id of the counter
	 * @param x - x position
	 * @param y - y position
	 * @param length - number of digits
	 * @param value - initial value
	 * @param color - font color
	 * @param scale - scale of the text
	 */
	void addTimer(int id,int x,int y,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	/*
	 * Set the value of the counter defined by the ID
	 *
	 * @param id - id of the counter
	 * @param value - the new value
	 */
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
};

}

