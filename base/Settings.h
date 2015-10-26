#pragma once
#include "..\utils\Color.h"

namespace ds {

	struct Settings {

		Color clearColor;
		int screenWidth;
		int screenHeight;
		int viewportWidth;
		int viewportHeight;
		bool fullScreen;
		bool synched;
		int mode;
		bool showEditor;

		Settings() {
			screenWidth = 1024;
			screenHeight = 768;
			viewportWidth = 1024;
			viewportHeight = 768;
			showEditor = false;
			clearColor = Color(100, 149, 237);
			fullScreen = false;
			synched = true;
			mode = 1;
		}
	};

}