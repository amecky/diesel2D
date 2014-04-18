#pragma once
#include "Converter.h"

namespace ds {

class BitmapFontConverter : public Converter {

public:
	BitmapFontConverter() : Converter("BitmapFontConverter") {}
	~BitmapFontConverter() {}
	void convert(const char* fileName);
};

}

