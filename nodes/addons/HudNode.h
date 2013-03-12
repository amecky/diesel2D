#pragma once
#include "..\Node.h"
#include "..\SimpleQuadNode.h"
#include "..\TextNode.h"
#include "..\..\addons\TextCounter.h"
#include "..\..\addons\StopWatch.h"

namespace ds {

class HudNode : public Node {

struct FloatingText {
	bool floating;
	float timer;
	float ttl;
	uint32 textIndex;
	Vec2 pos;
	float velocity;
	Vec2 size;
	float maxScale;
};

struct CounterEntry {
	IdString hashName;
	TextCounter* textCounter;
};

struct TimerEntry {
	IdString hashName;
	StopWatch* stopWatch;
};

typedef std::vector<CounterEntry> CounterEntries;
typedef std::vector<TimerEntry> TimerEntries;

public:
	HudNode(Renderer* renderer,int material,int bitmapFont);
	virtual ~HudNode();
	uint32 addImage(const Vec2& pos,const char* taItemName);
	uint32 addImage(const Vec2& pos,const Rect& rect,float textureSize = 1024.0f);
	uint32 addText(const Vec2& pos,const std::string& text,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	void updateText(uint32 id,const std::string& txt);
	Vec2 calculateTextSize(const std::string& text,float scale = 1.0f);
	void addCounter(const char* name,const Vec2& pos,uint32 digits,const Color& color,float scale = 1.0f);
	void activate();
	void deactivate();
	void setValue(const char* counterName,int value);
	void addValue(const char* counterName,int value);
	const int getValue(const char* counterName) const;
	void update(float elapsed);
	void addTimer(const char* name,const Vec2& pos,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	const int getMinutes(const char* timerName) const;
	const int getSeconds(const char* timerName) const;
	void resetTimer(const char* timerName);
	void startTimer(const char* timerName);
	void stopTimer(const char* timerName);
	//void resetAndStart(const char* timerName);
	// floating text
	void createFloatingText();
	void startFloatingText(const char* txt,const ds::Vec2& pos,float ttl,float maxScale);

	virtual void render();
private:
	StopWatch* findByName(const char* timerName);
	TextCounter* findTextCounterByName(const char* counterName);
	const TextCounter* findTextCounterByName(const char* counterName) const;
	void setState(bool active);
	SimpleQuadNode* m_MeshNode;
	TextNode* m_TextNode;
	CounterEntries m_CounterEntries;
	TimerEntries m_TimerEntries;
	bool m_SupportFloatingText;
	FloatingText m_FloatingText;
};

}