#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class ColorFadeToAction : public AbstractAction {

		struct ColorFadeToActionData {

			int num;
			int total;
			SID* ids;
			Color* startColors;
			Color* endColors;
			float* timers;
			float* ttl;
			tweening::TweeningType* tweeningTypes;
			int* modes;

			ColorFadeToActionData() : num(0), total(0) {}
		};

	public:
		ColorFadeToAction();
		~ColorFadeToAction();
		void attach(SID id,const Color& startColor,const Color& endColor,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);		
		void clear();
		void debug();
		void debug(SID sid) {}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		ColorFadeToActionData m_Data;
	};


}