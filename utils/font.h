#pragma once
#include "..\renderer\render_types.h"
#include "..\sprites\Sprite.h"
#include "..\renderer\BitmapFont.h"

namespace ds {

	namespace font {

		v2 calculateSize(const BitmapFont& bitmapFont, const char* text, int padding = 4, float scaleX = 1.0f, float scaleY = 1.0f);

		v2 calculateLimitedSize(const BitmapFont& bitmapFont, const char* text, int chars, int padding = 4, float scaleX = 1.0f, float scaleY = 1.0f);

		void createText(const BitmapFont& bitmapFont, const v2& pos, const char* text, const Color& color, std::vector<Sprite>& sprites, float scaleX = 1.0f, float scaleY = 1.0f);

		void createCharacter(const BitmapFont& bitmapFont, const v2& pos, char c, const Color& color, std::vector<Sprite>& sprites, float scaleX = 1.0f, float scaleY = 1.0f);
		
	}
}

