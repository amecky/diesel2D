#pragma once
#include "..\utils\Color.h"
#include "Vector.h"

namespace ds {

	namespace assets {

		int createTexture(int width,int height);

		int loadTexture(const char* name);

		int loadTextureBinary(const char* name);

		void reloadTextureBinary(const char* name);

		int loadTexture(const char* dirName,const char* name);

		int getTextureId(const char* name);

		int loadTextureWithColorKey(const char* name,const Color& color);

		void setTexture(int id,int index = 0);

		LPDIRECT3DTEXTURE9 getDirectTexture(int textureID);

		D3DLOCKED_RECT lockTexture(int id);

		void unlockTexture(int id);

		//void fillTexture(int id,const Vector2f& pos,int sizeX,int sizeY,Color* colors);

		Vector2f getTextureSize(int idx);

		int findFreeTextureSlot();

	}
}
