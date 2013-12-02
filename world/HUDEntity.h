#pragma once
#include "World.h"
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"
#include "..\nodes\SpriteBatch.h"

namespace ds {

class HUDEntity : public Entity {

struct HUDEntry {
	uint32 id;
	ds::Vec2 pos;
	ds::Color color;
	float scale;
	int flag;
	std::vector<Sprite> sprites;
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
	HUDEntity();
	virtual ~HUDEntity();
	void init(ds::Renderer* renderer,int textureID,const char* fontName);
	const EntityType getType() const {
		return ET_HUD;
	}
	void update(float elapsed);
	void draw();
	int addText(int x,int y,const std::string& txt,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	int addCounter(int x,int y,int length,int value,const ds::Color& color = ds::Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	void setCounterValue(int id,int value);
private:
	int createEntry(const ds::Vec2& pos,float scale = 1.0f,const ds::Color& color = ds::Color::WHITE);
	void createText(HUDEntry* entry,const std::string& text,bool clear = true);

	BitmapFont m_Font;
	ds::SpriteBatch* m_Buffer;
	HUDText m_TextEntries[32];
	HUDEntry m_HUDEntries[128];
	HUDCounter m_Counter[16];
	HUDTimer m_Timer[16];
};

}

