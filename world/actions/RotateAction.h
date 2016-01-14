#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class RotateAction : public AbstractAction {

		struct RotateActionData : public BasicData {

			float* angleVelocities;
			float* timers;
			float* ttl;			
			int* modes;

			RotateActionData() : BasicData() {}
		};

	public:
		RotateAction();
		virtual ~RotateAction() {
			if (m_Data.buffer != 0) {
				delete[] m_Data.buffer;
			}
		}
		void attach(SID id,float angleVelocity,float ttl,int mode = 0);
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
		RotateActionData m_Data;
	};

}