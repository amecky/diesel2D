#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class RemoveAfterAction : public AbstractAction {

		struct RemoveAfterActionData : public BasicData {

			float* timers;
			float* ttl;			

			RemoveAfterActionData() : BasicData() {}
		};

	public:
		RemoveAfterAction();
		virtual ~RemoveAfterAction() {
			if (m_Data.buffer != 0) {
				delete[] m_Data.buffer;
			}
		}
		void attach(SID id, float ttl);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
		void removeByID(SID id) {
			remove(id, m_Data);
		}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		RemoveAfterActionData m_Data;
	};

}