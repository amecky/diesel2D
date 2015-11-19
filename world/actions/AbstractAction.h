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
			virtual ~AbstractAction() {
				//if (m_Data.buffer != 0) {
					//delete[] m_Data.buffer;
				//}
			}
			virtual void update(SpriteArray& array,float dt,ActionEventBuffer& buffer) = 0;
			//const int size() const {
				//return m_Data.num;
			//}
			virtual void removeByID(SID id) = 0;
			void remove(SID id, const BasicData& data) {
				int idx = data.findIndex(id);
				if (idx != -1) {
					swap(idx);
				}
			}
			void removeByIndex(int i) {
				swap(i);
			}
			virtual void clear() = 0;
			virtual void debug() = 0;
			virtual void debug(SID sid) = 0;
			void setBoundingRect(const Rect& r) {
				m_BoundingRect = r;
			}
			virtual void allocate(int sz) = 0;
			int next(SID sid, BasicData& data) {
				if (data.total == 0) {
					int size = 256;
					allocate(size);
					data.num = 0;
				}
				if (data.num + 1 >= data.total) {
					// FIXME: resize
				}
				int idx = data.num;
				int tmp = data.findIndex(sid);
				if (tmp != -1) {
					idx = tmp;
				}
				else {
					++data.num;
				}
				return idx;
			}
			
		protected:
			virtual SID swap(int index) = 0;
			//std::map<SID,int> m_Mapping;
			//char* m_Buffer;
			Rect m_BoundingRect;
		};

}
