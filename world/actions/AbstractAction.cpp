#include "AbstractAction.h"
#include "..\..\utils\Log.h"

namespace ds {

	void AbstractAction::removeByIndex(int i) {
		swap(i);
	}

	void AbstractAction::setBoundingRect(const Rect& r) {
		m_BoundingRect = r;
	}

	int AbstractAction::find(SID id) {
		for (int i = 0; i < _buffer.size; ++i) {
			if (_ids[i] == id) {
				return i;
			}
		}
		return -1;
	}

	bool AbstractAction::contains(SID id) {
		return find(id) != -1;
	}
	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	SID AbstractAction::swap(int i) {
		SID current = _ids[i];
		_buffer.remove(i);
		return current;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AbstractAction::clear() {
		_buffer.size = 0;
	}

	void AbstractAction::removeByID(SID id) {
		int idx = find(id);
		if (idx != -1) {
			swap(idx);
		}
	}
}
