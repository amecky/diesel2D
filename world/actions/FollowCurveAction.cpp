#include "FollowCurveAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowCurveAction::FollowCurveAction() : AbstractAction() {
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowCurveAction::~FollowCurveAction() {
		if (m_Data.buffer != 0) {
			delete[] m_Data.buffer;
		}
	}

	void FollowCurveAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(BezierCurve*) + sizeof(float) + sizeof(float) + sizeof(int));
		m_Data.buffer = new char[size];
		m_Data.ids = (SID*)(m_Data.buffer);
		m_Data.path = (BezierCurve**)(m_Data.ids + sz);
		m_Data.timers = (float*)(m_Data.path + sz);
		m_Data.ttl = (float*)(m_Data.timers + sz);
		m_Data.modes = (int*)(m_Data.ttl + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowCurveAction::attach(SID id,BezierCurve* path,float ttl,int mode) {
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
	void FollowCurveAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {
		if ( m_Data.num > 0 ) {				
			// move
			Vector2f p;
			Vector2f t;
			for ( int i = 0; i < m_Data.num; ++i ) {
				float norm = math::norm(m_Data.timers[i] , m_Data.ttl[i]);
				m_Data.path[i]->get(norm,&p);
				m_Data.path[i]->getTangent(norm,&t);
				float a = vector::calculateRotation(t);
				sar::setPosition(array,m_Data.ids[i],p);
				sar::rotate(array,m_Data.ids[i],a);
				m_Data.timers[i] += dt;
				if ( m_Data.timers[i] >= m_Data.ttl[i] ) {
					if ( m_Data.modes[i] < 0 ) {
						m_Data.timers[i] = 0.0f;
					}
					else if ( m_Data.modes[i] == 0 ) {
						m_Data.path[i]->get(1.0f,&p);
						m_Data.path[i]->getTangent(1.0f,&t);
						sar::setPosition(array,m_Data.ids[i],p);
						float a = vector::calculateRotation(t);
						sar::rotate(array,m_Data.ids[i],a);
						buffer.add(m_Data.ids[i], AT_FOLLOW_CURVE, array.getType(m_Data.ids[i]));
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
	SID FollowCurveAction::swap(int i) {
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
	void FollowCurveAction::clear() {
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowCurveAction::debug() {
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