#include "FollowCurveAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowCurveAction::FollowCurveAction() : AbstractAction("follow_curve") {
		int sizes[] = { sizeof(SID), sizeof(BezierCurve*), sizeof(float), sizeof(float), sizeof(int) };
		_buffer.init(sizes, 5);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowCurveAction::~FollowCurveAction() {}

	void FollowCurveAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (SID*)_buffer.get_ptr(0);
			_path = (BezierCurve**)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
			_modes = (int*)_buffer.get_ptr(4);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowCurveAction::attach(SID id,BezierCurve* path,float ttl,int mode) {
		allocate(_buffer.size + 1);
		int idx = _buffer.size;
		_ids[idx] = id;
		_path[idx] = path;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		_modes[idx] = mode;
		if ( mode > 0 ) {
			--_modes[idx];
		}
		++_buffer.size;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowCurveAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			// move
			Vector2f p;
			Vector2f t;
			for (int i = 0; i < _buffer.size; ++i) {
				float norm = math::norm(_timers[i] , _ttl[i]);
				_path[i]->get(norm,&p);
				_path[i]->getTangent(norm,&t);
				float a = vector::calculateRotation(t);
				sar::setPosition(array,_ids[i],p);
				sar::rotate(array,_ids[i],a);
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					if ( _modes[i] < 0 ) {
						_timers[i] = 0.0f;
					}
					else if ( _modes[i] == 0 ) {
						_path[i]->get(1.0f,&p);
						_path[i]->getTangent(1.0f,&t);
						sar::setPosition(array,_ids[i],p);
						float a = vector::calculateRotation(t);
						sar::rotate(array,_ids[i],a);
						buffer.add(_ids[i], AT_FOLLOW_CURVE, array.getType(_ids[i]));
						removeByIndex(i);						
					}
					else {
						--_modes[i];
						_timers[i] = 0.0f;

					}
				}
			}
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowCurveAction::debug() {
		for (int i = 0; i < _buffer.size; ++i) {
			LOG << i << " : id: " << _ids[i] << " timer: " << _timers[i];
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