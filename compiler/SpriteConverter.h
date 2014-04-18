#pragma once
#include "Converter.h"

namespace ds {

class SpriteConverter : public Converter {

public:
	SpriteConverter(void);
	~SpriteConverter(void);
	void convert(const char* fileName);
};

}

