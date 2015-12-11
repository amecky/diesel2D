#pragma once
#include "..\renderer\shader.h"
#include "..\renderer\BitmapFont.h"

namespace ds {

	namespace assets {

		BitmapFont* loadFont(const char* name,int textureID);

		void loadSpriteTemplates();

		int loadShader(const char* fileName);


	}

}

