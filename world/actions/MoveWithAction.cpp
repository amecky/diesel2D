#include "MoveWithAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveWithAction::MoveWithAction() {
		
	}

	void MoveWithAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(MoveFunc) + sizeof(float) + sizeof(float));
		m_Data.buffer = new char[size];
		m_Data.ids = (SID*)(m_Data.buffer);
		m_Data.functions = (MoveFunc*)(m_Data.ids + sz);
		m_Data.timers = (float*)(m_Data.functions + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveWithAction::attach(SID id,const MoveFunc& function,float ttl) {
		int idx = next(id, m_Data);
		m_Data.ids[idx] = id;
		m_Data.timers[idx] = 0.0f;
		m_Data.functions[idx] = function;
		m_Data.ttl[idx] = ttl;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveWithAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {	
		if ( m_Data.num > 0 ) {				
			// move
			float angle = 0.0f;
			Vector2f p;
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = m_Data.timers[i] / m_Data.ttl[i];				
				m_Data.functions[i](p,&angle,norm);
				m_Data.timers[i] += dt;
				sar::setPosition(array,m_Data.ids[i],p);
				sar::rotate(array,m_Data.ids[i],angle);
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					m_Data.functions[i](p,&angle,1.0f);
					sar::setPosition(array,m_Data.ids[i],p);
					buffer.add(m_Data.ids[i], AT_MOVE_WITH, array.getType(m_Data.ids[i]));
					removeByIndex(i);					
				}
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID MoveWithAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.functions[i] = m_Data.functions[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveWithAction::clear() {
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveWithAction::debug() {
		LOG << "------- MoveWithAction -------";
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " id: " << m_Data.ids[i] << " timer: " << m_Data.timers[i];
		}		
	}

}