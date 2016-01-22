#include "ScaleByPathAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScaleByPathAction::ScaleByPathAction() : AbstractAction() {
		int sizes[] = { sizeof(SID), sizeof(Vector2fPath*), sizeof(float), sizeof(float) };
		_buffer.init(sizes, 4);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	ScaleByPathAction::~ScaleByPathAction() {
		
	}

	void ScaleByPathAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (SID*)_buffer.get_ptr(0);
			_path = (Vector2fPath**)_buffer.get_ptr(1);
			_timers = (float*)_buffer.get_ptr(2);
			_ttl = (float*)_buffer.get_ptr(3);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScaleByPathAction::attach(SpriteArray& array,SID id, Vector2fPath* path, float ttl) {
		allocate(_buffer.size + 1);
		int idx = _buffer.size;
		_ids[idx] = id;
		_path[idx] = path;
		v2 s(1, 1);
		path->get(0.0f, &s);
		sar::scale(array, id, s);
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		++_buffer.size;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScaleByPathAction::update(SpriteArray& array, float dt, ActionEventBuffer& buffer) {
		if ( _buffer.size > 0 ) {				
			// move
			v2 scale;
			for (int i = 0; i < _buffer.size; ++i) {
				float norm = math::norm(_timers[i], _ttl[i]);
				_path[i]->get(norm, &scale);
				sar::scale(array, _ids[i], scale);
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					_path[i]->get(1.0f, &scale);
					sar::scale(array, _ids[i], scale);
					buffer.add(_ids[i], AT_SCALE_BY_PATH, array.getType(_ids[i]));
					removeByIndex(i);
				}
			}
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void ScaleByPathAction::debug() {
		if ( _buffer.size > 0 ) {
			LOG << "---------- ScalingAction ---------- ";
		}
		for ( int i = 0; i < _buffer.size; ++i ) {
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