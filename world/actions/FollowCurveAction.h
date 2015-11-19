#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\math\BezierCurve.h"

namespace ds {

	class FollowCurveAction : public AbstractAction {

		struct FollowCurveActionData : public BasicData {

			BezierCurve** path;
			float* timers;
			float* ttl;
			int* modes;

			FollowCurveActionData() : BasicData() {}
		};

	public:
		FollowCurveAction();
		virtual ~FollowCurveAction();
		void attach(SID id,BezierCurve* path,float ttl,int mode = 0);
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
		FollowCurveActionData m_Data;
	};

}