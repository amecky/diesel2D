#pragma once
#include "..\utils\Color.h"

namespace ds {

struct Settings {
	int width;
	int height;
	LPCSTR title;
	int mode;
	bool synched;
	Color clearColor;
	bool fullscreen;
	bool postProcessing;
	bool readPlainResources;
	Settings() { 
		width = 800; 
		height = 800; 
		title = "Title"; 
		mode = 0; 
		synched = false; 
		clearColor = Color(0.0f,0.0f,0.0f,1.0f); 
		fullscreen = false; 
		postProcessing = false;
		readPlainResources = true;
	}
};

};

