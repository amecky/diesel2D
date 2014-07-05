#pragma once
#include "Converter.h"

namespace ds {

class ParticleManagerConverter : public Converter {

public:
	ParticleManagerConverter(void);
	~ParticleManagerConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\particles";
	}
};

}