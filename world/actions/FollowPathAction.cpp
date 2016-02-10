#include "FollowPathAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowPathAction::FollowPathAction() : AbstractAction("follow_path") {
		int sizes[] = { sizeof(SID), sizeof(CubicBezierPath*), sizeof(float), sizeof(float), sizeof(int) };
		_buffer.init(sizes, 5);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	FollowPathAction::~FollowPathAction() {}

	void FollowPathAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (SID*)_buffer.get_ptr(0);
			_path = (CubicBezierPath**)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
			_modes = (int*)_buffer.get_ptr(4);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void FollowPathAction::attach(SID id,CubicBezierPath* path,float ttl,int mode) {
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
	void FollowPathAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {
		if (_buffer.size > 0) {
			// move
			v2 p;
			v2 t;
			for (int i = 0; i < _buffer.size; ++i) {
				float norm = math::norm(_timers[i] , _ttl[i]);
				_path[i]->approx(norm,&p);

				_path[i]->tanget(norm, &t);
				float a = vector::calculateRotation(t);
				sar::rotate(array, _ids[i], a);
				sar::setPosition(array,_ids[i],p);
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					if ( _modes[i] < 0 ) {
						_timers[i] = 0.0f;
					}
					else if ( _modes[i] == 0 ) {
						_path[i]->get(1.0f,&p);
						sar::setPosition(array,_ids[i],p);						
						buffer.add(_ids[i], AT_FOLLOW_PATH, array.getType(_ids[i]));
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
	void FollowPathAction::debug() {
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