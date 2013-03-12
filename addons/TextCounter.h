#pragma once
#include "..\nodes\TextNode.h"

namespace ds {

class TextCounter {

public:
	TextCounter(TextNode* textNode,const Vec2& position,int digits,const Color& color = Color(1.0f,1.0f,1.0f,1.0f),float scale = 1.0f);
	~TextCounter(void);
	void reset();
	void add(int value);
	void set(int value);
	int getValue() const {
		return m_Value;
	}
	void enableAutoScaling(float startScale,float scaleMultiplier,float ttl);
	void startScaling();
	void update(float elapsed);
private:
	TextNode* m_TextNode;
	int m_Digits;
	int m_Value;
	float m_Timer;
	uint32 m_ValueIdx;
	bool m_Active;
	// scaling
	bool m_Scaling;
	float m_ScaleTimer;
	float m_ScaleTTL;
	float m_ScalueMultiplier;
	float m_ScaleStart;
};

}
