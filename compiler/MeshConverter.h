#pragma once
#include "Converter.h"

namespace ds {

class MeshConverter : public Converter {

public:
	MeshConverter(void);
	~MeshConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\resources";
	}
};

}
