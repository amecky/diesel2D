#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class RotateAction : public AbstractAction {

		struct RotateActionData {
			int num;
			int total;
			SID* ids;
			float* angleVelocities;
			float* timers;
			float* ttl;			
			int* modes;

			RotateActionData() : num(0) , total(0) {}
		};

	public:
		RotateAction();
		virtual ~RotateAction() {}
		void attach(SID id,float angleVelocity,float ttl,int mode = 0);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid);
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		RotateActionData m_Data;
	};

}