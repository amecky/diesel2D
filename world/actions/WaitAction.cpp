#include "WaitAction.h"
#include "..\..\math\GameMath.h"
#include "..\..\utils\Log.h"

namespace ds {
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	WaitAction::WaitAction() : AbstractAction() {
		int sizes[] = { sizeof(SID), sizeof(float), sizeof(float) };
		_buffer.init(sizes, 3);
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	WaitAction::~WaitAction() {
	}

	void WaitAction::allocate(int sz) {
		if (_buffer.resize(sz)) {
			_ids = (SID*)_buffer.get_ptr(0);
			_timers = (float*)_buffer.get_ptr(1);
			_ttl = (float*)_buffer.get_ptr(2);
		}
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WaitAction::attach(SID id,float ttl) {
		allocate(_buffer.size + 1);
		int idx = _buffer.size;
		_ids[idx] = id;
		_timers[idx] = 0.0f;
		_ttl[idx] = ttl;
		++_buffer.size;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WaitAction::update(SpriteArray& array,float dt,ActionEventBuffer& buffer) {
		if ( _buffer.size > 0 ) {				
			// move
			for (int i = 0; i < _buffer.size; ++i) {
				_timers[i] += dt;
				if ( _timers[i] >= _ttl[i] ) {
					buffer.add(_ids[i],AT_WAIT,array.getType(_ids[i]));
					removeByIndex(i);	
				}
			}
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID WaitAction::swap(int i) {
		SID current = _ids[i];
		_buffer.remove(i);
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WaitAction::clear() {
		//m_Mapping.clear();
		_buffer.size = 0;
	}

	void WaitAction::removeByID(SID id) {
		int idx = find(id, _ids, _buffer.size);
		if (idx != -1) {
			swap(idx);
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void WaitAction::debug() {
		if (_buffer.size > 0) {
			LOG << "---------- WaitAction ---------- ";
		}
		for (int i = 0; i < _buffer.size; ++i) {
			LOG << i << " : id: " << _ids[i] << " ttl: " << _ttl[i] << " timer: " << _timers[i];
		}
	}

	void WaitAction::debug(SID sid) {
		//int i = m_Mapping[sid];
		int i = find(sid, _ids, _buffer.size);
		if (i != -1) {
			LOG << "> wait : id: " << _ids[i] << " ttl: " << _ttl[i] << " timer: " << _timers[i];
		}
	}

}