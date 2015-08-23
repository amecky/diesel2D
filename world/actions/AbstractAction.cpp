#include "AbstractAction.h"
#include "..\..\utils\Log.h"

namespace ds {

	AbstractAction::AbstractAction(void) : m_Buffer(0) {		
		//m_BoundingRect.left = 0.0f;
		//m_BoundingRect.bottom = 0.0f;
		//m_BoundingRect.right = renderer::getScreenWidth();
		//m_BoundingRect.top = renderer::getScreenHeight();
	}


	AbstractAction::~AbstractAction(void) {
		if ( m_Buffer != 0 ) {
			delete[] m_Buffer;
		}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AbstractAction::setBoundingRect(const Rect& r) {
		m_BoundingRect = r;
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	const int AbstractAction::size() const {
		return m_Mapping.size();
	}

	// -------------------------------------------------------
	// Check if Mapping contains SID
	// -------------------------------------------------------
	const bool AbstractAction::contains(SID sid) const {
		 return m_Mapping.find(sid) != m_Mapping.end();
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AbstractAction::remove(SID id) {
		//LOG << "removing id: " << id;
		if ( m_Mapping.find(id) != m_Mapping.end()) {
			int i = m_Mapping[id];
			SID sid = swap(i);
			//LOG << "removed: " << sid;
			m_Mapping.erase(id);
		}
		//else {
			//LOG << "not found";
		//}
	}

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	void AbstractAction::removeByIndex(int i) {		
		SID current = swap(i);
		//LOG << "remove index: " << i << " current: " << current;
		m_Mapping.erase(m_Mapping.find(current));
	}

}
