#pragma once
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"

namespace ds {

	namespace font {

		Vec2 calculateSize(const BitmapFont& bitmapFont,const std::string& text,float scaleX = 1.0f,float scaleY = 1.0f);

		void createText(const BitmapFont& bitmapFont,const Vec2& pos,const std::string& text,const Color& color,std::vector<Sprite>& sprites,float scaleX = 1.0f,float scaleY = 1.0f);

		bool load(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));

		bool load2(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));

		bool loadBinary(const char* name,Renderer* renderer,int textureId,BitmapFont& font,const Color& fillColor = Color(1.0f,0.0f,1.0f,1.0f));
	}
}

