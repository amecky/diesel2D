#pragma once
#include "..\lib\collection_types.h"
#include <map>
#include "Sprite.h"
#include "..\ui\IMGUI.h"

namespace ds {

	class SpriteGroup {

		typedef Array<Sprite> Sprites;

	public:
		SpriteGroup(const char* name);
		~SpriteGroup();
		void load();
		void save();
		void render();
		void showDialog();
	private:
		Sprites _sprites;
		int _state;
		IdString _name;
		gui::ComponentModel<SID> _model;
		int _offset;
		v2 _position;
		SID _currentSID;
	};


	class SpriteGroupContainer {

		typedef std::map<const char*, SpriteGroup*> GroupMapping;

		public:
			SpriteGroupContainer() {
				_state = 1;
				_offset = 0;
				_position = v2(512, 384);
			}
			~SpriteGroupContainer();
			SpriteGroup* get(const char* name);
			SpriteGroup* create(const char* name);
			bool contains(const char* name) const;
			void load();
			void save();
			void showDialog();
		private:
			GroupMapping _map;
			gui::ComponentModel<SpriteGroup*> _model;
			v2 _position;
			int _state;
			int _offset;

	};
}
