#pragma once
#include "..\nodes\TextNode.h"

namespace ds {

class StopWatch {

public:
	StopWatch(TextNode* textNode,const Vec2& position,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	~StopWatch(void);
	void start();
	void stop();
	void reset();
	int getMinutes() const;
	int getSeconds() const;
	void update(float elapsed);
private:
	TextNode* m_TextNode;
	int m_Seconds;
	int m_Minutes;
	float m_Timer;
	uint32 m_TimeIdx;
	bool m_Active;
};

}

