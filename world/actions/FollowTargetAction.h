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

		struct FollowTargetActionData {
			int num;
			int total;
			SID* ids;
			float* velocities;

			FollowTargetActionData() : num(0), total(0) {}
		};

	public:
		FollowTargetAction();
		virtual ~FollowTargetAction();
		void attach(SID id,float velocity);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
		void setTarget(Vector2f* target) {
			m_Target = target;
		}
	protected:
		SID swap(int i);
	private:
		int computeSeparation(SpriteArray& array, int self, float radius, Vector2f* v);
		int findNearest(SpriteArray& array, const Vector2f& pos, float radius, int self, int type);
		Vector2f* m_Target;
		void allocate(int sz);
		FollowTargetActionData m_Data;
	};

}