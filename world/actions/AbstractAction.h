#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include <map>
#include "..\World.h"
#include "..\..\math\tweening.h"

namespace ds {

	class AbstractAction {

		public:
			AbstractAction();
			virtual ~AbstractAction();
			virtual void update(SpriteArray& array,float dt,ActionEventBuffer& buffer) = 0;
			const int size() const;
			void remove(SID id);
			void removeByIndex(int i);
			virtual void clear() = 0;
			const bool contains(SID sid) const;
			virtual void debug() = 0;
			virtual void debug(SID sid) = 0;
			void setBoundingRect(const Rect& r);
		protected:
			virtual SID swap(int index) = 0;
			std::map<SID,int> m_Mapping;
			char* m_Buffer;
			Rect m_BoundingRect;
		};

}
