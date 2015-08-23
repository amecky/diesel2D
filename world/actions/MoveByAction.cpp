#include "MoveByAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveByAction::MoveByAction() {
		
	}

	void MoveByAction::allocate(int sz) {
		int size = sz * (sizeof(SID) + sizeof(Vector2f) + sizeof(float) + sizeof(bool));
		m_Buffer = new char[size];
		m_Data.ids = (SID*)(m_Buffer);
		m_Data.velocities = (Vector2f*)(m_Data.ids + sz);
		m_Data.timers = (float*)(m_Data.velocities + sz);
		m_Data.bounce = (bool*)(m_Data.timers + sz);
		m_Data.total = sz;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::attach(SID id,const Vector2f& velocity,bool bounce) {
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
		m_Data.velocities[idx] = velocity;		
		m_Data.timers[idx] = 0.0f;
		m_Data.bounce[idx] = bounce;
		m_Mapping[id] = idx;
	}

	void MoveByAction::rotateTo(SpriteArray& array, int index) {
		float angle = vector::calculateRotation(m_Data.velocities[index]);
		sar::rotate(array, m_Data.ids[index], angle);
	}

	bool MoveByAction::isOutOfBounds(const Vector2f& pos, const Vector2f& v) {
		if (v.x > 0.0f && pos.x > m_BoundingRect.right) {
			return true;
		}
		if (v.x < 0.0f && pos.x < m_BoundingRect.left) {
			return true;
		}
		if (v.y > 0.0f && pos.y > m_BoundingRect.top) {
			return true;
		}
		if (v.y < 0.0f && pos.y < m_BoundingRect.bottom) {
			return true;
		}
		return false;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {	
		//Viewport& vp = renderer::getSelectedViewport();
		//const Vector2f& ws = vp.getWorldSize();
		if ( m_Data.num > 0 ) {				
			// move
			for ( int i = 0; i < m_Data.num; ++i ) {
				Vector2f p = sar::getPosition(array,m_Data.ids[i]);
				p += m_Data.velocities[i] * dt;
				//float angle = vector::calculateRotation(m_Data.velocities[i]);
				//sar::rotate(array,m_Data.ids[i], angle);
				//if ( p.x < 0.0f || p.x > ws.x || p.y < 0.0f || p.y > ws.y ) {
				//if (p.x < m_BoundingRect.left || p.x > m_BoundingRect.right || p.y < m_BoundingRect.top || p.y > m_BoundingRect.bottom) {
				if (isOutOfBounds(p, m_Data.velocities[i])) { 
					if (m_Data.bounce[i]) {
						if (p.y < 0.0f || p.y > m_BoundingRect.bottom) {
							m_Data.velocities[i].y *= -1.0f;
						}
						if (p.x < 0.0f || p.x > m_BoundingRect.right ) {
							m_Data.velocities[i].x *= -1.0f;
						}
						rotateTo(array, i);
					}
					else {
						buffer.add(m_Data.ids[i], AT_MOVE_BY, array.getType(m_Data.ids[i]));
					}
				}
				else {
					sar::setPosition(array, m_Data.ids[i], p);
				}
				m_Data.timers[i] += dt;
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID MoveByAction::swap(int i) {
		int last = m_Data.num - 1;
		SID last_id = m_Data.ids[last];
		SID current = m_Data.ids[i];
		m_Data.ids[i] = m_Data.ids[last];
		m_Data.velocities[i] = m_Data.velocities[last];
		m_Data.timers[i] = m_Data.timers[last];
		m_Data.bounce[i] = m_Data.bounce[last];
		m_Mapping[last_id] =  i;		
		--m_Data.num;
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::clear() {
		m_Mapping.clear();
		m_Data.num = 0;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::debug() {
		LOG << "------- MoveByAction -------";
		for ( int i = 0; i < m_Data.num; ++i ) {
			LOG << i << " id: " << m_Data.ids[i] << " velocity: " << DBG_V2(m_Data.velocities[i]) << " timer: " << m_Data.timers[i];
		}		
	}

}