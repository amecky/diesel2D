#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class MoveToAction : public AbstractAction {

		struct MoveToActionData : public BasicData {

			Vector2f* startPositions;
			Vector2f* endPositions;
			float* timers;
			float* ttl;
			tweening::TweeningType* tweeningTypes;
			int* modes;

			MoveToActionData() : BasicData() {}
		};

	public:
		MoveToAction();
		virtual ~MoveToAction() {
			if (m_Data.buffer != 0) {
				delete[] m_Data.buffer;
			}
		}
		void attach(SID id, SpriteArray& array,const Vector2f& startPos, const Vector2f& endPos, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
		void removeByID(SID id) {
			remove(id, m_Data);
		}
		int size() const {
			return m_Data.num;
		}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		MoveToActionData m_Data;
	};

}