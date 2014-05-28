#pragma once
#include "Converter.h"

namespace ds {

class ScriptConverter : public Converter {

public:
	ScriptConverter(void);
	~ScriptConverter(void);
	void convert(const char* fileName);
};

}