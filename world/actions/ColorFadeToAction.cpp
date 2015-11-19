#include "ColorFadeToAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ColorFadeToAction::ColorFadeToAction() : AbstractAction() {
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ColorFadeToAction::~ColorFadeToAction() {
		if (m_Data.buffer != 0) {
			delete[] m_Data.buffer;
		}
	}

	void ColorFadeToAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(Color) + sizeof(Color) + sizeof(float) + sizeof(float) + sizeof(tweening::TweeningType)+sizeof(int));
		m_Data.buffer = new char[size];
		m_Data.ids = (SID*)(m_Data.buffer);
		m_Data.startColors = (Color*)(m_Data.ids + sz);
		m_Data.endColors = (Color*)(m_Data.startColors + sz);
		m_Data.timers = (float*)(m_Data.endColors + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.tweeningTypes = (tweening::TweeningType*)(m_Data.ttl + sz);
		m_Data.modes = (int*)(m_Data.tweeningTypes + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ColorFadeToAction::attach(SID id, const Color& startColor, const Color& endColor, float ttl, int mode, const tweening::TweeningType& tweeningType) {
		int idx = next(id, m_Data);
		m_Data.ids[idx] = id;
		m_Data.startColors[idx] = startColor;
		m_Data.endColors[idx] = endColor;
		m_Data.timers[idx] = 0.0f;
		m_Data.ttl[idx] = ttl;
		m_Data.tweeningTypes[idx] = tweeningType;
		m_Data.modes[idx] = mode;
		if ( mode > 0 ) {
			--m_Data.modes[idx];
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ColorFadeToAction::update(SpriteArray& array, float dt, ActionEventBuffer& buffer) {
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = math::norm(m_Data.timers[i] , m_Data.ttl[i]);
				Color clr;
				clr.r = tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startColors[i].r, m_Data.endColors[i].r, norm);
				clr.g = tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startColors[i].g, m_Data.endColors[i].g, norm);
				clr.b = tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startColors[i].b, m_Data.endColors[i].b, norm);
				clr.a = tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startColors[i].a, m_Data.endColors[i].a, norm);
				sar::setColor(array,m_Data.ids[i],clr);
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						sar::setColor(array, m_Data.ids[i], m_Data.endColors[i]);
						removeByIndex(i);
					}
					else {
						--m_Data.modes[i];
						m_Data.timers[i] = 0.0f;

					}
				}
			}
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID ColorFadeToAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.startColors[i] = m_Data.startColors[last];
		m_Data.endColors[i] = m_Data.endColors[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		m_Data.tweeningTypes[i] = m_Data.tweeningTypes[last];
		m_Data.modes[i] = m_Data.modes[last];
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ColorFadeToAction::clear() {
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ColorFadeToAction::debug() {
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " : id: " << m_Data.ids[i] << " timer: " << m_Data.timers[i];
		}
		/*
		std::map<SID,int>::iterator it = m_Mapping.begin();
		while ( it != m_Mapping.end()) {
			LOG << it->first << " = " << it->second;
			++it;
		}
		*/
	}

}