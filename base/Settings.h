#pragma once
#include "..\utils\Color.h"

namespace ds {

	struct Settings {

		Color clearColor;
		int screenWidth;
		int screenHeight;
		bool initializeEditor;
		//int viewportWidth;
		//int viewportHeight;
		bool fullScreen;
		bool synched;
		int mode;

		Settings() {
			screenWidth = 1024;
			screenHeight = 768;
			//viewportWidth = 1024;
			//viewportHeight = 768;
			clearColor = Color(100, 149, 237);
			fullScreen = false;
			synched = true;
			initializeEditor = false;
			mode = 1;
		}
	};

}