#include "AbstractAction.h"
#include "..\..\utils\Log.h"

namespace ds {

	void AbstractAction::remove(SID id, const BasicData& data) {
		int idx = data.findIndex(id);
		if (idx != -1) {
			swap(idx);
		}
	}
	void AbstractAction::removeByIndex(int i) {
		swap(i);
	}
	void AbstractAction::setBoundingRect(const Rect& r) {
		m_BoundingRect = r;
	}
	int AbstractAction::next(SID sid, BasicData& data) {
		if (data.total == 0) {
			int size = 256;
			allocate(size);
			data.num = 0;
		}
		if (data.num + 1 >= data.total) {
			// FIXME: resize
		}
		int idx = data.num;
		int tmp = data.findIndex(sid);
		if (tmp != -1) {
			idx = tmp;
		}
		else {
			++data.num;
		}
		return idx;
	}
}
