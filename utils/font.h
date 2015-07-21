#pragma once
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"
#include "..\sprites\Sprite.h"

namespace ds {

	namespace font {

		Vector2f calculateSize(const BitmapFont& bitmapFont,const std::string& text,int padding = 4,float scaleX = 1.0f,float scaleY = 1.0f);

		void createText(const BitmapFont& bitmapFont,const Vector2f& pos,const std::string& text,const Color& color,std::vector<Sprite>& sprites,float scaleX = 1.0f,float scaleY = 1.0f);

		void createCharacter(const BitmapFont& bitmapFont, const Vector2f& pos, char c, const Color& color, std::vector<Sprite>& sprites, float scaleX = 1.0f, float scaleY = 1.0f);
		
	}
}

