#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class AlphaFadeToAction : public AbstractAction {

		struct AlphaFadeToActionData {

			int num;
			int total;
			SID* ids;
			float* startAlphas;
			float* endAlphas;
			float* timers;
			float* ttl;
			tweening::TweeningType* tweeningTypes;
			int* modes;

			AlphaFadeToActionData() : num(0) , total(0) {}
		};

	public:
		AlphaFadeToAction();
		~AlphaFadeToAction();
		void attach(SID id,float startAlpha,float endAlpha,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
		void update(SpriteArray& array,float dt,ActionEventBuffer& buffer);		
		void clear();
		void debug();
		void debug(SID sid) {}
	protected:
		SID swap(int index);
	private:
		void allocate(int sz);
		AlphaFadeToActionData m_Data;
	};


}