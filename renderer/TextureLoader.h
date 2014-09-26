#pragma once
#include "..\io\Serializer.h"
#include "assets.h"

namespace ds {

class TextureLoader : public BinarySerializer {

public:
	TextureLoader(void);
	~TextureLoader(void);
	virtual int load(const char* fileName) {
		return assets::loadTextureBinary(fileName);
	}
	virtual const char* getResourceDirectory()   {
		return "content\\textures";
	}

};

}