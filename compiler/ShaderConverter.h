#pragma once
#include "Converter.h"

namespace ds {

class ShaderConverter : public Converter {

public:
	ShaderConverter(void);
	~ShaderConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\shaders";
	}
};

}
