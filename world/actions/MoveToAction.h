#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class MoveToAction : public AbstractAction {

		struct MoveToActionData {
			int num;
			int total;
			SID* ids;
			Vector2f* startPositions;
			Vector2f* endPositions;
			float* timers;
			float* ttl;			
			tweening::TweeningType* tweeningTypes;
			int* modes;

			MoveToActionData() : num(0) , total(0) {}
		};

	public:
		MoveToAction();
		virtual ~MoveToAction() {}
		void attach(SID id, SpriteArray& array,const Vector2f& startPos, const Vector2f& endPos, float ttl, int mode = 0, const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		MoveToActionData m_Data;
	};

}