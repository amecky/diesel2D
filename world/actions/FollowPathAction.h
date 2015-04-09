#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\math\CubicBezierPath.h"

namespace ds {

	class FollowPathAction : public AbstractAction {

		struct FollowPathActionData {
			int num;
			int total;
			SID* ids;
			CubicBezierPath** path;
			float* timers;
			float* ttl;
			int* modes;

			FollowPathActionData() : num(0) , total(0) {}
		};

	public:
		FollowPathAction();
		virtual ~FollowPathAction();
		void attach(SID id,CubicBezierPath* path,float ttl,int mode = 0);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
	protected:
		SID swap(int i);
	private:
		void allocate(int sz);
		FollowPathActionData m_Data;
	};

}