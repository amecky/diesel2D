#pragma once
#include "Converter.h"

namespace ds {

class SpritesDescriptionConverter : public Converter {

public:
	SpritesDescriptionConverter(void);
	~SpritesDescriptionConverter(void);
	void convert(const char* fileName);
};

}

