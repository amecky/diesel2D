#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class MoveWithAction : public AbstractAction {

		struct MoveWithActionData {
			int num;
			int total;
			SID* ids;
			MoveFunc* functions;
			float* timers;
			float* ttl;
			MoveWithActionData() : num(0) , total(0) {}
		};

	public:
		MoveWithAction();
		virtual ~MoveWithAction() {}
		void attach(SID id,const MoveFunc& function,float ttl);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		MoveWithActionData m_Data;
	};

}