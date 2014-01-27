#pragma once
#include "..\dxstdafx.h"

namespace ds {

class GameTimer {

public:
	GameTimer() : m_Millis(0) , m_Seconds(0) , m_Minutes(0) , m_Hours(0) , m_Forward(true) , m_Active(true) , m_Timer(0.0f) {}
	~GameTimer() {}
	void tick(float elapsed) {
		if ( m_Active ) {
			m_Timer += elapsed;
			float millis = elapsed * 1000.0f;
			m_Millis += static_cast<uint32>(millis);
			if ( m_Millis > 1000 ) {
				m_Millis -= 1000;
			}
			if ( m_Timer >= 1.0f ) {
				m_Timer = 0.0f;
				++m_Seconds;
				if ( m_Seconds >= 60 ) {
					m_Seconds = 0;
					++m_Minutes;
					if ( m_Minutes >= 60 ) {
						m_Minutes = 0;
						++m_Hours;
					}
				}
			}
		}
	}
	void start() {
		m_Active = true;
	}
	void stop() {
		m_Active = false;
	}
	const bool isActive() const {
		return m_Active;
	}
	const uint32 getSeconds() const {
		return m_Seconds;
	}
	const uint32 getMinutes() const {
		return m_Minutes;
	}
	const uint32 getHours() const {
		return m_Hours;
	}
	const uint32 getTimeInMillis() const {
		return m_Millis + m_Seconds * 1000 + m_Minutes * 60000;
	}
	void set(uint32 millis = 0,uint32 seconds = 0,uint32 minutes = 0,uint32 hours = 0) {
		m_Millis = millis;
		m_Seconds = seconds;
		m_Minutes = minutes;
		m_Hours = hours;
		m_Timer = 0.0f;
	}
	void reset() {
		set(0,0,0,0);
	}
private:
	float m_Timer;
	bool m_Forward;
	bool m_Active;
	uint32 m_Millis;
	uint32 m_Seconds;
	uint32 m_Minutes;
	uint32 m_Hours;
};

}
