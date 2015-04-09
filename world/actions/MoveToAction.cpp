#include "MoveToAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveToAction::MoveToAction() {
		
	}

	void MoveToAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(Vector2f) + sizeof(Vector2f) + sizeof(float) + sizeof(float) + sizeof(tweening::TweeningType)+sizeof(int));
		m_Buffer = new char[size];
		m_Data.ids = (SID*)(m_Buffer);
		m_Data.startPositions = (Vector2f*)(m_Data.ids + sz);
		m_Data.endPositions = (Vector2f*)(m_Data.startPositions + sz);
		m_Data.timers = (float*)(m_Data.endPositions + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.tweeningTypes = (tweening::TweeningType*)(m_Data.ttl + sz);
		m_Data.modes = (int*)(m_Data.tweeningTypes + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveToAction::attach(SID id,const Vector2f& startPos,const Vector2f& endPos,float ttl,int mode,const tweening::TweeningType& tweeningType) {
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
		m_Data.startPositions[idx] = startPos;
		m_Data.endPositions[idx] = endPos;
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
	void MoveToAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {	
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = math::norm(m_Data.timers[i] , m_Data.ttl[i]);
				sar::setPosition(array,m_Data.ids[i],tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startPositions[i], m_Data.endPositions[i], norm));
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						sar::setPosition(array,m_Data.ids[i],tweening::interpolate(m_Data.tweeningTypes[i], m_Data.startPositions[i], m_Data.endPositions[i], 1.0f));
						buffer.add(m_Data.ids[i], AT_MOVE_TO, array.getType(m_Data.ids[i]));
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
	SID MoveToAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.startPositions[i] = m_Data.startPositions[last];
		m_Data.endPositions[i] = m_Data.endPositions[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		m_Data.tweeningTypes[i] = m_Data.tweeningTypes[last];
		m_Data.modes[i] = m_Data.modes[last];
		m_Mapping[last_id] =  i;		
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveToAction::clear() {
		m_Mapping.clear();
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveToAction::debug() {
		LOG << "------- MoveToAction -------";
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " id: " << m_Data.ids[i] << " start: " << DBG_V2(m_Data.startPositions[i]) << " end: " << DBG_V2(m_Data.endPositions[i]) << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
		}		
	}

	void MoveToAction::debug(SID sid) {
		int i = m_Mapping[sid];
		LOG << "> move_to : id: " << m_Data.ids[i] << " start: " << DBG_V2(m_Data.startPositions[i]) << " end: " << DBG_V2(m_Data.endPositions[i]) << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
	}

}