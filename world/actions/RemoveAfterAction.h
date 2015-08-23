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

		struct RemoveAfterActionData {
			int num;
			int total;
			SID* ids;
			float* timers;
			float* ttl;			

			RemoveAfterActionData() : num(0), total(0) {}
		};

	public:
		RemoveAfterAction();
		virtual ~RemoveAfterAction() {}
		void attach(SID id, float ttl);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		RemoveAfterActionData m_Data;
	};

}