#include "AssetCompiler.h"
#include "..\io\FileWatcher.h"
#include "..\utils\Log.h"
#include "..\utils\FileUtils.h"
#include "..\objects\BloomComponent.h"
#include "..\renderer\TextureLoader.h"
#include "..\particles\ParticleManager.h"

//const unsigned int UINT32_MAX = 1024;

namespace ds {

	namespace assets {

		BitmapFont* loadFont(const char* name,int textureID) {
			BitmapFont* font = renderer::createBitmapFont(name,textureID);
			if (!font->load()) {
				font->importJSON();
				font->save();
			}
			renderer::initializeBitmapFont(font, textureID);
			return font;
		}

		void loadSpriteTemplates() {
			renderer::loadSpriteTemplates();
		}
	
	}

}
