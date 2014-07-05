#pragma once
#include "Converter.h"

namespace ds {

class SpritesDescriptionConverter : public Converter {

public:
	SpritesDescriptionConverter(void);
	~SpritesDescriptionConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\resources";
	}
};

}

