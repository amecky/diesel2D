#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\math\tweening.h"
#include "..\World.h"
#include "..\..\math\tweening.h"
#include "..\..\lib\BlockArray.h"
#include "..\..\io\ReportWriter.h"

namespace ds {

	struct AbstractActionDefinition {

		virtual void read(const JSONReader& reader, int category_id) = 0;
		virtual ActionType getActionType() = 0;
	};

	class AbstractAction {

		public:
			AbstractAction() {}
			virtual ~AbstractAction() {}
			virtual void update(SpriteArray& array,float dt,ActionEventBuffer& buffer) = 0;
			void removeByIndex(int i);
			virtual void debug() = 0;
			virtual void debug(SID sid) = 0;
			void setBoundingRect(const Rect& r);
			virtual void allocate(int sz) = 0;
			virtual ActionType getActionType() const = 0;
			virtual void attach(SID sid, AbstractActionDefinition* definition) {}// = 0;
			void clear();
			void removeByID(SID id);
			bool contains(SID id);
			virtual void save(const ReportWriter& writer) {}
		protected:
			int find(SID id);
			SID swap(int index);
			Rect m_BoundingRect;
			BlockArray _buffer;
			SID* _ids;
		};

	

}
