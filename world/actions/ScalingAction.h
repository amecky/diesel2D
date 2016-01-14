#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class ScalingAction : public AbstractAction {

		struct ScalingActionData : public BasicData {

			Vector2f* startScale;
			Vector2f* endScale;
			float* timers;
			float* ttl;
			tweening::TweeningType* tweeningTypes;
			int* modes;

			ScalingActionData() : BasicData() {}
		};

	public:
		ScalingAction();
		virtual ~ScalingAction();
		void attach(SID id,const Vector2f& startScale,const Vector2f& endScale,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
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
		ScalingActionData m_Data;
	};

}