#include "MoveByAction.h"
#include "..\..\utils\Log.h"
#include "..\..\math\GameMath.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	MoveByAction::MoveByAction() {
		int sizes[] = { sizeof(SID), sizeof(v2), sizeof(float), sizeof(bool) };
		_buffer.init(sizes, 4);
	}

	void MoveByAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (SID*)_buffer.get_ptr(0);
			_velocities = (v2*)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_bounce = (bool*)_buffer.get_ptr(3);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::attach(SID id,const Vector2f& velocity,bool bounce) {		
		allocate(_buffer.size + 1);
		int idx = _buffer.size;
		_ids[idx] = id;
		_velocities[idx] = velocity;
		_timers[idx] = 0.0f;
		_bounce[idx] = bounce;
		++_buffer.size;
	}

	void MoveByAction::rotateTo(SpriteArray& array, int index) {
		float angle = vector::calculateRotation(_velocities[index]);
		sar::rotate(array, _ids[index], angle);
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

	void MoveByAction::bounce(SID sid, BounceDirection direction) {
		for (int i = 0; i < _buffer.size; ++i) {
			if (_ids[i] == sid) {
				if (direction == BD_Y || direction == BD_BOTH) {
					_velocities[i].y *= -1.0f;
				}
				if (direction == BD_X || direction == BD_BOTH) {
					_velocities[i].x *= -1.0f;
				}
			}
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {	
		if (_buffer.size > 0) {
			for (int i = 0; i < _buffer.size; ++i) {
				Vector2f p = sar::getPosition(array, _ids[i]);
				p += _velocities[i] * dt;
				if (isOutOfBounds(p, _velocities[i])) {
					if (_bounce[i]) {
						if (p.y < m_BoundingRect.bottom || p.y > m_BoundingRect.top) {
							_velocities[i].y *= -1.0f;
						}
						if (p.x < 0.0f || p.x > m_BoundingRect.right ) {
							_velocities[i].x *= -1.0f;
						}
						rotateTo(array, i);
						p += _velocities[i] * dt;
						sar::setPosition(array, _ids[i], p);
					}
					else {
						buffer.add(_ids[i], AT_MOVE_BY, array.getType(_ids[i]));
					}
				}
				else {
					sar::setPosition(array, _ids[i], p);
				}				
				_timers[i] += dt;
			}
		}
	}
	
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::debug() {
		LOG << "------- MoveByAction -------";
		for (int i = 0; i < _buffer.size; ++i) {
			LOG << i << " id: " << _ids[i] << " velocity: " << DBG_V2(_velocities[i]) << " timer: " << _timers[i];
		}		
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::debug(SID sid) {
		int i = find(sid);
		if (i != -1) {
			LOG << "MoveByAction - id: " << _ids[i] << " velocity: " << DBG_V2(_velocities[i]) << " timer: " << _timers[i];
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void MoveByAction::save(const ReportWriter& writer) {
		if (_buffer.size > 0) {
			writer.addSubHeader("MoveByAction");
			const char* HEADERS[] = { "Index", "ID", "Velocity", "Timer" ,"Bounce"};
			writer.startTable(HEADERS, 5);
			for (int i = 0; i < _buffer.size; ++i) {
				writer.startRow();
				writer.addCell(i);
				writer.addCell(_ids[i]);
				writer.addCell(_velocities[i]);
				writer.addCell(_timers[i]);
				writer.addCell(_bounce[i]);
				writer.endRow();
			}
			writer.endTable();
		}
	}

}