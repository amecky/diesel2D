#pragma once
#include "..\utils\GameMath.h"

namespace ds {

	typedef unsigned int GCID;

	const GCID GC_NONE = 65535;

	struct GUICtrl {
		GCID parent;
		Vector2f position;
		Vector2f dimension;
		GUICtrl* next;
		int index;
		GUICtrl();
	};

	class IGUICtrlRenderer {
		public:
			IGUICtrlRenderer(GUICtrl* control);
			virtual ~IGUICtrlRenderer();
			virtual void update() = 0;
		protected:
			GUICtrl* m_Control;
	};
}
