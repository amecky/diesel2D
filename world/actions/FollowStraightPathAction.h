#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\math\StraightPath.h"

namespace ds {

	class FollowStraightPathAction : public AbstractAction {

		struct FollowStraightPathActionData {
			int num;
			int total;
			SID* ids;
			StraightPath** path;
			float* timers;
			float* ttl;
			int* modes;

			FollowStraightPathActionData() : num(0), total(0) {}
		};

	public:
		FollowStraightPathAction();
		virtual ~FollowStraightPathAction();
		void attach(SID id, SpriteArray& array,StraightPath* path, float ttl, int mode = 0);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
	protected:
		SID swap(int i);
	private:
		void allocate(int sz);
		FollowStraightPathActionData m_Data;
	};

}