#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class RotateByAction : public AbstractAction {

		struct RotateByActionData : public BasicData {

			float* startAngles;
			float* endAngles;
			float* steps;
			float* timers;
			float* ttl;			
			tweening::TweeningType* tweeningTypes;
			int* modes;

			RotateByActionData() : BasicData() {}
		};

	public:
		RotateByAction();
		virtual ~RotateByAction() {
			if (m_Data.buffer != 0) {
				delete[] m_Data.buffer;
			}
		}
		void attach(SID id,float startAngle,float endAngle,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
		void removeByID(SID id) {
			remove(id, m_Data);
		}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		RotateByActionData m_Data;
	};

}