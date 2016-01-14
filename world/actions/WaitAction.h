#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\lib\BlockArray.h"

namespace ds {

	class WaitAction : public AbstractAction {

	public:
		WaitAction();
		virtual ~WaitAction();
		void attach(SID id,float ttl);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
		void removeByID(SID id);
	protected:
		SID swap(int i);
	private:
		void allocate(int sz);
		BlockArray _buffer;
		SID* _ids;
		float* _timers;
		float* _ttl;
	};

}