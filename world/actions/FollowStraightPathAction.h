#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\math\StraightPath.h"

namespace ds {

	class FollowStraightPathAction : public AbstractAction {

		struct FollowStraightPathActionData : public BasicData {

			StraightPath** path;
			float* timers;
			float* ttl;
			int* modes;

			FollowStraightPathActionData() : BasicData() {}
		};

	public:
		FollowStraightPathAction();
		virtual ~FollowStraightPathAction();
		void attach(SID id, SpriteArray& array,StraightPath* path, float ttl, int mode = 0);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
		void removeByID(SID id) {
			remove(id, m_Data);
		}
	protected:
		SID swap(int i);
	private:
		void allocate(int sz);
		FollowStraightPathActionData m_Data;
	};

}