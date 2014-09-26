#include "assets.h"
#include "Renderer.h"

namespace ds {

	namespace assets {

		// -------------------------------------------------------
		// Find free texture slot
		// -------------------------------------------------------
		int findFreeTextureSlot() {
			for ( int i = 0; i < MAX_TEXTURES; ++i ) {
				if ( renderContext.textures[i].flags == 0 ) {
					return i;
				}
			}
			return -1;
		}

		// -------------------------------------------------------
		// Create empty texture
		// -------------------------------------------------------
		int createTexture(int width,int height) {
			int id = findFreeTextureSlot();
			if ( id != -1 ) {
				TextureAsset* tr = &renderContext.textures[id];
				tr->name = string::murmur_hash("xxxx");
				tr->flags = 1;
				HR(renderContext.device->CreateTexture( width, height,1, 0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&tr->texture,NULL ));
				tr->width = width;
				tr->height = height;		
				// clear the texture with white color		
				D3DLOCKED_RECT sRect;
				tr->texture->LockRect(0, &sRect, NULL, NULL);
				BYTE *bytes = (BYTE*)sRect.pBits;
				memset(bytes, 128, width*sRect.Pitch);
				tr->texture->UnlockRect(0);
			}
			return id;
		}

		// -------------------------------------------------------
		// Creates and loads a texture from specific directory
		// -------------------------------------------------------
		int loadTexture(const char* dirName,const char* name) {
			int id = findFreeTextureSlot();
			if ( id != -1 ) {
				TextureAsset* tr = &renderContext.textures[id];
				tr->name = string::murmur_hash(name);
				tr->flags = 1;
				int lw = D3DX_DEFAULT;
				int lh = D3DX_DEFAULT;		
				D3DXIMAGE_INFO imageInfo;
				char fileName[256];
				sprintf(fileName,"%s\\%s.png",dirName,name);
				LOGC("Renderer") << "Trying to load texture " << fileName;
				HR(D3DXCreateTextureFromFileEx(renderContext.device,fileName, 0, 0, 1, 0,
					D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL,&tr->texture));
				tr->width = imageInfo.Width;
				tr->height = imageInfo.Height;		
				LOGC("Renderer") << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
				return id;
			}
			else {
				LOGEC("Renderer") << "No more texture slots available";
				return -1;
			}
		}

		// -------------------------------------------------------
		// Creates and loads a texture from specific directory
		// -------------------------------------------------------
		int loadTextureBinary(const char* fileName) {
			int id = findFreeTextureSlot();
			if ( id != -1 ) {
				TextureAsset* tr = &renderContext.textures[id];
				tr->name = string::murmur_hash(fileName);
				tr->flags = 1;
				int lw = D3DX_DEFAULT;
				int lh = D3DX_DEFAULT;		
				D3DXIMAGE_INFO imageInfo;
				LOGC("Renderer") << "Trying to load texture " << fileName;
				HR(D3DXCreateTextureFromFileEx(renderContext.device,fileName, 0, 0, 1, 0,
					D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL,&tr->texture));
				tr->width = imageInfo.Width;
				tr->height = imageInfo.Height;		
				LOGC("Renderer") << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
				return id;
			}
			else {
				LOGEC("Renderer") << "No more texture slots available";
				return -1;
			}
		}

		// -------------------------------------------------------
		// Creates and loads a texture from specific directory
		// -------------------------------------------------------
		void reloadTextureBinary(const char* fileName) {
			IdString hash = string::murmur_hash(fileName);
			// check if we have one
			int id = getTextureId(fileName);
			if ( id == -1 ) {
				loadTextureBinary(fileName);
			}
			TextureAsset* tr = &renderContext.textures[id];
			SAFE_RELEASE(tr->texture);
			tr->flags = 1;
			int lw = D3DX_DEFAULT;
			int lh = D3DX_DEFAULT;		
			D3DXIMAGE_INFO imageInfo;
			LOGC("Renderer") << "Trying to load texture " << fileName;
			HR(D3DXCreateTextureFromFileEx(renderContext.device,fileName, 0, 0, 1, 0,
				D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, 0x000000, &imageInfo, NULL,&tr->texture));
			tr->width = imageInfo.Width;
			tr->height = imageInfo.Height;		
			LOGC("Renderer") << "ID: " << id << " Width: " << imageInfo.Width << " Height: " << imageInfo.Height << " mip levels " << imageInfo.MipLevels << " Format: " << imageInfo.Format;
				
		}

		// -------------------------------------------------------
		// Creates and loads a texture
		// -------------------------------------------------------
		int loadTexture(const char* name) {
			return loadTexture("content\\textures",name);
		}
		
		// -------------------------------------------------------
		// Get texture id
		// -------------------------------------------------------
		int getTextureId(const char* name) {
			IdString hash = string::murmur_hash(name);
			for ( int i = 0; i < MAX_TEXTURES; ++i ) {
				TextureAsset* tr = &renderContext.textures[i];
				if ( tr->flags == 1 && tr->name == hash ) {
					return i;
				}
			}
			return -1;
		}

		// -------------------------------------------------------
		// Get texture size
		// -------------------------------------------------------
		Vector2f getTextureSize(int idx) {
			// FIXME: check if valid id
			assert(idx >= 0 && idx < MAX_TEXTURES);
			TextureAsset* tr = &renderContext.textures[idx];
			assert(tr->flags != 0);
			return Vector2f(static_cast<int>(tr->width),static_cast<int>(tr->height));
		}

		// -------------------------------------------------------
		// get direct texture pointer
		// -------------------------------------------------------
		LPDIRECT3DTEXTURE9 getDirectTexture(int textureID) {
			assert(textureID < MAX_TEXTURES);
			return renderContext.textures[textureID].texture;
		}

		// -------------------------------------------------------
		// lock texture
		// -------------------------------------------------------
		D3DLOCKED_RECT lockTexture(int id) {
			D3DLOCKED_RECT lockedRect;
			ZeroMemory(&lockedRect, sizeof(lockedRect));
			TextureAsset* tr = &renderContext.textures[id];
			HR(tr->texture->LockRect(0,&lockedRect,NULL,0));	
			return lockedRect;
		}

		// -------------------------------------------------------
		// unlock texture
		// -------------------------------------------------------
		void unlockTexture(int id) {
			TextureAsset* tr = &renderContext.textures[id];
			HR(tr->texture->UnlockRect(0));	
		}

		// -------------------------------------------------------
		// fill texture with color array
		// -------------------------------------------------------
		/*
		void fillTexture(int id,const Vector2f& pos,int sizeX,int sizeY,Color* colors) {
			D3DLOCKED_RECT r = lockTexture(id);
			uchar * pBits = (uchar *)r.pBits;
			Color* current = colors;
			for ( int x = 0; x < sizeX; ++x ) {
				for ( int y = 0; y < sizeY; ++y ) {			
					*pBits   = current->b * 255.0f;
					++pBits;
					*pBits = current->g * 255.0f;
					++pBits;
					*pBits = current->r * 255.0f;
					++pBits;
					*pBits = current->a * 255.0f;
					++pBits;
					++current;
				}
			}
			unlockTexture(id);
		}
		*/
	}
}
