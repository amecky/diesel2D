#include "RotateAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	RotateAction::RotateAction() {
		
	}

	void RotateAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(int));
		m_Data.buffer = new char[size];
		m_Data.ids = (SID*)(m_Data.buffer);
		m_Data.angleVelocities = (float*)(m_Data.ids + sz);
		m_Data.timers = (float*)(m_Data.angleVelocities + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.modes = (int*)(m_Data.ttl + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateAction::attach(SID id,float angleVelocity,float ttl,int mode) {
		int idx = next(id, m_Data);
		m_Data.ids[idx] = id;
		m_Data.angleVelocities[idx] = DEGTORAD(angleVelocity);
		m_Data.timers[idx] = 0.0f;
		m_Data.ttl[idx] = ttl;
		m_Data.modes[idx] = mode;
		if ( mode > 0 ) {
			--m_Data.modes[idx];
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {	
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				float angle = sar::getRotation(array,m_Data.ids[i]);
				angle += m_Data.angleVelocities[i] * dt;
				sar::rotate(array, m_Data.ids[i], angle);
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						removeByIndex(i);
					}
					else {
						float current = sar::getRotation(array,m_Data.ids[i]);
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
	SID RotateAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.angleVelocities[i] = m_Data.angleVelocities[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		m_Data.modes[i] = m_Data.modes[last];
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateAction::clear() {
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void RotateAction::debug() {
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " : id: " << m_Data.ids[i] << " angle velocity: " << m_Data.angleVelocities[i] << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
		}
	}

	void RotateAction::debug(SID sid) {
		int i = m_Data.findIndex(sid);
		LOG << i << " : id: " << m_Data.ids[i] << " angle velocity: " << m_Data.angleVelocities[i] << " ttl: " << m_Data.ttl[i] << " timer: " << m_Data.timers[i];
	}

}