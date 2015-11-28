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

	class FollowTargetAction : public AbstractAction {

		struct FollowTargetActionData : public BasicData {

			float* velocities;
			SID* targets;

			FollowTargetActionData() : BasicData() {}
		};

	public:
		FollowTargetAction();
		virtual ~FollowTargetAction();
		void attach(SID id,SID target,float velocity);
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
		int computeSeparation(SpriteArray& array, int self, float radius, Vector2f* v);
		int findNearest(SpriteArray& array, const Vector2f& pos, float radius, int self, int type);
		void allocate(int sz);
		FollowTargetActionData m_Data;
	};

}