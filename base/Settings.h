#pragma once
#include "..\utils\Color.h"

namespace ds {

	struct Settings {

		Color clearColor;
		int screenWidth;
		int screenHeight;
		bool fullScreen;
		bool synched;
		int mode;

		Settings() {
			int screenWidth = 1024;
			int screenHeight = 768;
			clearColor = Color(100, 149, 237);
			fullScreen = false;
			synched = true;
			mode = 1;
		}
	};

}