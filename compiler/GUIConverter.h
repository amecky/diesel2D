#pragma once
#include "Converter.h"

namespace ds {

class GUIConverter : public Converter {

public:
	GUIConverter(void);
	~GUIConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\resources";
	}
};

}
