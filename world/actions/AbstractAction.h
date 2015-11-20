#pragma once
#include "..\..\lib\DataContainer.h"
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "..\..\math\tweening.h"

namespace ds {

	struct BasicData {
	
		int num;
		int total;
		char* buffer;
		SID* ids;

		BasicData() : num(0), total(0), buffer(0) {}

		int findIndex(SID sid) const {
			for (int i = 0; i < num; ++i) {
				if (ids[i] == sid) {
					return i;
				}
			}
			return -1;
		}

		const bool contains(SID sid) const {
			return findIndex(sid) != -1;
		}

	};

	class AbstractAction {

		public:
			AbstractAction() {}
			virtual ~AbstractAction() {}
			virtual void update(SpriteArray& array,float dt,ActionEventBuffer& buffer) = 0;
			virtual void removeByID(SID id) = 0;
			void remove(SID id, const BasicData& data);
			void removeByIndex(int i);
			virtual void clear() = 0;
			virtual void debug() = 0;
			virtual void debug(SID sid) = 0;
			void setBoundingRect(const Rect& r);
			virtual void allocate(int sz) = 0;
			int next(SID sid, BasicData& data);
		protected:
			virtual SID swap(int index) = 0;
			Rect m_BoundingRect;
		};

}
