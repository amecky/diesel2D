#pragma once
#include "..\..\renderer\render_types.h"
#include "..\World.h"
#include "AbstractAction.h"
#include "..\..\math\FloatArray.h"

namespace ds {

	class ScaleByPathAction : public AbstractAction {

	public:
		ScaleByPathAction();
		virtual ~ScaleByPathAction();
		void attach(SpriteArray& array,SID id, Vector2fPath* path, float ttl);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);
		void clear();
		void debug();
		void debug(SID sid) {}
		void removeByID(SID id);
		ActionType getActionType() const {
			return AT_SCALE_BY_PATH;
		}
	protected:
		SID swap(int i);
	private:
		void allocate(int sz);
		Vector2fPath** _path;
		float* _timers;
		float* _ttl;
	};

}