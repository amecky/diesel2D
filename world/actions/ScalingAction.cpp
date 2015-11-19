#include "ScalingAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScalingAction::ScalingAction() : AbstractAction() {
		//int size = 256;
		//allocate(size);
		//m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScalingAction::~ScalingAction() {
		if (m_Data.buffer != 0) {
			delete[] m_Data.buffer;
		}
	}

	void ScalingAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(Vector2f) + sizeof(Vector2f) + sizeof(float) + sizeof(float)+sizeof(tweening::TweeningType) + sizeof(int));
		m_Data.buffer = new char[size];
		m_Data.ids = (SID*)(m_Data.buffer);
		m_Data.startScale = (Vector2f*)(m_Data.ids + sz);
		m_Data.endScale = (Vector2f*)(m_Data.startScale + sz);
		m_Data.timers = (float*)(m_Data.endScale + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.tweeningTypes = (tweening::TweeningType*)(m_Data.ttl + sz);
		m_Data.modes = (int*)(m_Data.tweeningTypes+sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScalingAction::attach(SID id,const Vector2f& startScale,const Vector2f& endScale,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		int idx = next(id, m_Data);
		m_Data.ids[idx] = id;
		m_Data.startScale[idx] = startScale;
		m_Data.endScale[idx] = endScale;
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
	void ScalingAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = math::norm(m_Data.timers[i] , m_Data.ttl[i]);
				sar::scale(array,m_Data.ids[i],tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startScale[i], m_Data.endScale[i], norm));
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						sar::scale(array,m_Data.ids[i],tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startScale[i], m_Data.endScale[i], 1.0f));
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
	SID ScalingAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.startScale[i] = m_Data.startScale[last];
		m_Data.endScale[i] = m_Data.endScale[last];
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
	void ScalingAction::clear() {
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScalingAction::debug() {
		if ( m_Data.num > 0 ) {
			LOG << "---------- ScalingAction ---------- ";
		}
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