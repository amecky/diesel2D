#pragma once
#include "Converter.h"

namespace ds {

class HUDConverter : public Converter {

public:
	HUDConverter(void);
	~HUDConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\resources";
	}
};

}

