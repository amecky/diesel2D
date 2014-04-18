#pragma once
#include "Converter.h"

namespace ds {

class GUIConverter : public Converter {

public:
	GUIConverter(void);
	~GUIConverter(void);
	void convert(const char* fileName);
};

}
