#include "RemoveAfterAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RemoveAfterAction::RemoveAfterAction() {
		
	}

	void RemoveAfterAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(float) + sizeof(float));
		m_Buffer = new char[size];
		m_Data.ids = (SID*)(m_Buffer);
		m_Data.timers = (float*)(m_Data.ids + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::attach(SID id, float ttl) {
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
		m_Data.timers[idx] = 0.0f;
		m_Data.ttl[idx] = ttl;
		m_Mapping[id] = idx;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::update(SpriteArray& array, float dt, ActionEventBuffer& buffer) {
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					buffer.add(m_Data.ids[i], AT_KILL, array.getType(m_Data.ids[i]));
					removeByIndex(i);
				}
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID RemoveAfterAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		m_Mapping[last_id] =  i;		
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::clear() {
		m_Mapping.clear();
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RemoveAfterAction::debug() {
		LOG << "------- MoveToAction -------";
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " id: " << m_Data.ids[i] << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
		}		
	}

	void RemoveAfterAction::debug(SID sid) {
		int i = m_Mapping[sid];
		LOG << "> move_to : id: " << m_Data.ids[i] << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
	}

}