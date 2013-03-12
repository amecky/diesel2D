#include "StopWatch.h"
#include "..\utils\StringUtils.h"

namespace ds {

StopWatch::StopWatch(ds::TextNode* textNode,const Vec2& position,const Color& color,float scale) : m_TextNode(textNode) , m_Active(false) , m_Timer(0.0f) {
	m_TimeIdx = m_TextNode->addText(position,"00:00",color,scale);
	reset();
}


StopWatch::~StopWatch(void) {
}

void StopWatch::start() {
	m_Active = true;
}

void StopWatch::stop() {
	m_Active = false;
}

void StopWatch::reset() {
	m_Minutes = 0;
	m_Seconds = 0;
	m_Timer = 0.0f;
	m_TextNode->updateText(m_TimeIdx,"00:00");
}

int StopWatch::getMinutes() const {
	return m_Minutes;
}

int StopWatch::getSeconds() const {
	return m_Seconds;
}

void StopWatch::update(float elapsed) {
	if ( m_Active ) {
		m_Timer += elapsed;
		if ( m_Timer >= 1.0f ) {
			m_Timer = 0.0f;
			++m_Seconds;
			if ( m_Seconds == 60 ) {
				++m_Minutes;
				m_Seconds = 0;
			}
			std::string timeStr;
			string::formatTime(m_Minutes,m_Seconds,timeStr);		
			m_TextNode->updateText(m_TimeIdx,timeStr);
		}
	}
}

}
