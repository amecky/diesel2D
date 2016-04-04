#pragma once
#include <stdint.h>
#include "..\lib\collection_types.h"
#include "EventStream.h"

namespace ds {

	struct Shortcut {
		const char* label;
		char key;
		uint32_t eventType;
	};

	class ShortcutsHandler {

	public:
		ShortcutsHandler(EventStream* events);
		~ShortcutsHandler();
		void debug();
		void handleInput(char ascii);
		void add(const char* label, char key, uint32_t eventType);
		void showDialog();
	private:
		EventStream* _events;
		Array<Shortcut> _shortcuts;
		Array<const char*> _model;
		v2 _position;
	};

}

