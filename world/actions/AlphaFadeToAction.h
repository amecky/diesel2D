#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "AbstractAction.h"

namespace ds {

	class AlphaFadeToAction : public AbstractAction {

		struct AlphaFadeToActionData : public BasicData {

			float* startAlphas;
			float* endAlphas;
			float* timers;
			float* ttl;
			tweening::TweeningType* tweeningTypes;
			int* modes;

			AlphaFadeToActionData() : BasicData() {}
		};

	public:
		AlphaFadeToAction();
		~AlphaFadeToAction();
		void attach(SID id,float startAlpha,float endAlpha,float ttl,int mode = 0,const tweening::TweeningType& tweeningType = &tweening::easeOutQuad);
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
		AlphaFadeToActionData m_Data;
	};


}