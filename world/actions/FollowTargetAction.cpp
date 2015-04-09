#include "FollowTargetAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowTargetAction::FollowTargetAction() : AbstractAction() , m_Target(0) {

	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowTargetAction::~FollowTargetAction() {
	}

	void FollowTargetAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(float));
		m_Buffer = new char[size];
		m_Data.ids = (SID*)(m_Buffer);
		m_Data.velocities = (float*)(m_Data.ids + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowTargetAction::attach(SID id,float velocity) {
		if ( m_Data.total == 0 ) {
			allocate(256);
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
		m_Data.velocities[idx] = velocity;
		m_Mapping[id] = idx;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowTargetAction::update(SpriteArray& array, float dt, ActionEventBuffer& buffer) {
		if ( m_Target !=0 && m_Data.num > 0 ) {				
			Vector2f p;
			Vector2f t;
			for ( int i = 0; i < m_Data.num; ++i ) {
				Vector2f p = sar::getPosition(array,m_Data.ids[i]);
				Vector2f diff = *m_Target - p;
				if (sqr_length(diff) > 100.0f) {
					Vector2f n = normalize(diff);
					n *= m_Data.velocities[i] * dt;
					p += n;
					sar::setPosition(array, m_Data.ids[i], p);
					float angle = vector::calculateRotation(n);
					sar::rotate(array, m_Data.ids[i], angle);
				}
			}

			Vector2f v;
			for (int i = 0; i < m_Data.num; ++i) {
				int in = array.getIndex(m_Data.ids[i]);
				int cnt = computeSeparation(array, i, 20.0f, &v);
				if (cnt > 0) {
					Vector2f n = normalize(v);
					array.positions[in] += n * m_Data.velocities[in] * dt * 0.75f;
				}
				/*
				int idx = findNearest(array,array.positions[in], 20.0f, in,array.types[in]);
				if (idx != -1) {					
					Vector2f p = array.positions[idx];
					Vector2f diff = array.positions[in] - p;
					if (sqr_length(diff) < 400.0f) {
						Vector2f n = normalize(diff);
						//n *= 0.5f;
						n *= m_Data.velocities[i] * dt;
						array.positions[in] += n;
						//n = normalize(diff);
						//n *= -1.0f * m_Data.velocities[idx] * dt;
						array.positions[idx] -= n;
					}
				}
				*/
			}
			
		}
	}

	// -------------------------------------------------------
	// find nearest enemy to current enemy
	// -------------------------------------------------------
	int FollowTargetAction::findNearest(SpriteArray& array,const Vector2f& pos, float radius, int self,int type) {
		int ret = -1;
		float min = 2000.0f * 2000.0f;
		for (int i = 0; i < array.num; ++i) {
			if (i != self && array.types[i] == type) {
				Vector2f diff = pos - array.positions[i];
				float distSq = sqr_length(diff);
				if (distSq < (radius + radius)*(radius + radius)) {
					if (distSq < min) {
						ret = i;
						min = distSq;
					}
				}
			}
		}
		return ret;
	}

	// -------------------------------------------------------
	// find nearest enemy to current enemy
	// -------------------------------------------------------
	int FollowTargetAction::computeSeparation(SpriteArray& array, int self, float radius,Vector2f* v) {
		int count = 0;
		v->x = 0.0f;
		v->y = 0.0f;
		int type = array.types[self];
		Vector2f pos = array.positions[self];
		for (int i = 0; i < array.num; ++i) {
			if (i != self && array.types[i] == type) {
				Vector2f diff = pos - array.positions[i];
				//float distSq = sqr_length(diff);
				if (sqr_length(diff) < (radius + radius)*(radius + radius)) {
					*v += diff;
					++count;
				}
			}
		}
		*v *= -1.0f;
		if (count > 0) {
			v->x /= count;
			v->y /= count;
		}
		return count;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID FollowTargetAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.velocities[i] = m_Data.velocities[last];
		m_Mapping[last_id] =  i;
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowTargetAction::clear() {
		m_Mapping.clear();
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowTargetAction::debug() {
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " : id: " << m_Data.ids[i] << " velocity: " << m_Data.velocities[i];
		}
		std::map<SID,int>::iterator it = m_Mapping.begin();
		while ( it != m_Mapping.end()) {
			LOG << it->first << " = " << it->second;
			++it;
		}
	}

}