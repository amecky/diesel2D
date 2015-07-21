#pragma once
#include "Converter.h"

namespace ds {

class DescriptorConverter : public Converter {

public:
	DescriptorConverter(void);
	~DescriptorConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\resources";
	}
};

}
