#pragma once
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"
#include "..\sprites\SpriteObject.h"

namespace ds {

	namespace font {

		Vector2f calculateSize(const BitmapFont& bitmapFont,const std::string& text,int padding = 4,float scaleX = 1.0f,float scaleY = 1.0f);

		void createText(const BitmapFont& bitmapFont,const Vector2f& pos,const std::string& text,const Color& color,std::vector<SpriteObject>& sprites,float scaleX = 1.0f,float scaleY = 1.0f);

		bool load(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));

		bool load2(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));

		void debug(BitmapFont& font);

		bool loadBinary(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));
	}
}

