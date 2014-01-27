#pragma once
#include "..\renderer\render_types.h"

namespace ds {

class AssetManager {

typedef std::vector<Sprite*> Sprites;

public:
	AssetManager(void);
	~AssetManager(void);
	bool loadSpriteSettings();
private:
	Sprites m_Sprites;
};

}

