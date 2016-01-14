#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"
#include "..\..\lib\BlockArray.h"

namespace ds {

	class MoveByAction : public AbstractAction {

	public:
		MoveByAction();
		virtual ~MoveByAction() {}
		void attach(SID id,const Vector2f& velocity,bool bounce = false);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
		void removeByID(SID id);
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		void rotateTo(SpriteArray& array, int index);
		bool isOutOfBounds(const Vector2f& pos, const Vector2f& v);

		BlockArray _buffer;
		SID* _ids;
		v2* _velocities;
		float* _timers;
		bool* _bounce;
	};

}