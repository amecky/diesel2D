#include "FollowPathAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowPathAction::FollowPathAction() : AbstractAction() {
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowPathAction::~FollowPathAction() {
		if (m_Data.buffer != 0) {
			delete[] m_Data.buffer;
		}
	}

	void FollowPathAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(CubicBezierPath*) + sizeof(float) + sizeof(float) + sizeof(int));
		m_Data.buffer = new char[size];
		m_Data.ids = (SID*)(m_Data.buffer);
		m_Data.path = (CubicBezierPath**)(m_Data.ids + sz);
		m_Data.timers = (float*)(m_Data.path + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.modes = (int*)(m_Data.ttl + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowPathAction::attach(SID id,CubicBezierPath* path,float ttl,int mode) {
		int idx = next(id, m_Data);
		m_Data.ids[idx] = id;
		m_Data.path[idx] = path;
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
	void FollowPathAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {
		if ( m_Data.num > 0 ) {				
			// move
			v2 p;
			v2 t;
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = math::norm(m_Data.timers[i] , m_Data.ttl[i]);
				m_Data.path[i]->approx(norm,&p);

				m_Data.path[i]->tanget(norm, &t);
				float a = vector::calculateRotation(t);
				sar::rotate(array, m_Data.ids[i], a);
				sar::setPosition(array,m_Data.ids[i],p);
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						m_Data.path[i]->get(1.0f,&p);
						sar::setPosition(array,m_Data.ids[i],p);						
						buffer.add(m_Data.ids[i], AT_FOLLOW_PATH, array.getType(m_Data.ids[i]));
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
	SID FollowPathAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.path[i] = m_Data.path[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.ttl[i] = m_Data.ttl[last];
		m_Data.modes[i] = m_Data.modes[last];
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowPathAction::clear() {
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowPathAction::debug() {
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