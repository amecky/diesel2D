#pragma once
#include "Converter.h"

namespace ds {

class HUDConverter : public Converter {

public:
	HUDConverter(void);
	~HUDConverter(void);
	void convert(const char* fileName);
};

}

