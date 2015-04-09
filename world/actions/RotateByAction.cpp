#include "RotateByAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RotateByAction::RotateByAction() {
		
	}

	void RotateByAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(tweening::TweeningType)+sizeof(int));
		m_Buffer = new char[size];
		m_Data.ids = (SID*)(m_Buffer);
		m_Data.startAngles = (float*)(m_Data.ids + sz);
		m_Data.endAngles = (float*)(m_Data.startAngles + sz);
		m_Data.steps = (float*)(m_Data.endAngles + sz);
		m_Data.timers = (float*)(m_Data.steps + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.tweeningTypes = (tweening::TweeningType*)(m_Data.ttl + sz);
		m_Data.modes = (int*)(m_Data.tweeningTypes + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateByAction::attach(SID id,float startAngle,float endAngle,float ttl,int mode,const tweening::TweeningType& tweeningType) {
		if ( m_Data.total == 0 ) {
			int size = 256;
			allocate(size);
			m_Data.num = 0;
		}
		int idx = m_Data.num;
		if ( m_Mapping.find(id) != m_Mapping.end()) {
			idx = m_Mapping[id];			
		}
		else {
			++m_Data.num;
		}
		m_Data.ids[idx] = id;
		m_Data.startAngles[idx] = startAngle;
		m_Data.endAngles[idx] = endAngle;
		m_Data.steps[idx] = endAngle - startAngle;
		m_Data.timers[idx] = 0.0f;
		m_Data.ttl[idx] = ttl;
		m_Data.tweeningTypes[idx] = tweeningType;
		m_Data.modes[idx] = mode;
		if ( mode > 0 ) {
			--m_Data.modes[idx];
		}
		m_Mapping[id] = idx;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateByAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {	
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = math::norm(m_Data.timers[i] , m_Data.ttl[i]);
				sar::rotate(array,m_Data.ids[i],tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startAngles[i], m_Data.endAngles[i], norm));
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						sar::rotate(array,m_Data.ids[i],tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startAngles[i], m_Data.endAngles[i], 1.0f));
						removeByIndex(i);
					}
					else {
						float current = sar::getRotation(array,m_Data.ids[i]);
						m_Data.startAngles[i] = current;
						m_Data.endAngles[i] = current + m_Data.steps[i];
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
	SID RotateByAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.startAngles[i] = m_Data.startAngles[last];
		m_Data.endAngles[i] = m_Data.endAngles[last];
		m_Data.steps[i] = m_Data.steps[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		m_Data.modes[i] = m_Data.modes[last];
		m_Data.tweeningTypes[i] = m_Data.tweeningTypes[last];
		m_Mapping[last_id] =  i;		
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateByAction::clear() {
		m_Mapping.clear();
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateByAction::debug() {
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " : id: " << m_Data.ids[i] << " start: " << m_Data.startAngles[i] << " end: " << m_Data.endAngles[i] << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
		}
	}

	void RotateByAction::debug(SID sid) {
		int i = m_Mapping[sid];
		LOG << "> rotate by : id: " << m_Data.ids[i] << " start: " << m_Data.startAngles[i] << " end: " << m_Data.endAngles[i] << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
	}

}