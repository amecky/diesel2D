#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class WaitAction : public AbstractAction {

		struct WaitActionData {
			int num;
			int total;
			SID* ids;
			float* timers;
			float* ttl;

			WaitActionData() : num(0) , total(0) {}
		};

	public:
		WaitAction();
		virtual ~WaitAction();
		void attach(SID id,float ttl);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
	protected:
		SID swap(int i);
	private:
		void allocate(int sz);
		WaitActionData m_Data;
	};

}