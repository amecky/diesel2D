#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class MoveByAction : public AbstractAction {

		struct MoveByActionData : public BasicData {

			Vector2f* velocities;
			float* timers;
			bool* bounce;

			MoveByActionData() : BasicData() {}
		};

	public:
		MoveByAction();
		virtual ~MoveByAction() {
			if (m_Data.buffer != 0) {
				delete[] m_Data.buffer;
			}
		}
		void attach(SID id,const Vector2f& velocity,bool bounce = false);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
		void removeByID(SID id) {
			remove(id, m_Data);
		}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		void rotateTo(SpriteArray& array, int index);
		MoveByActionData m_Data;
		bool isOutOfBounds(const Vector2f& pos, const Vector2f& v);
	};

}