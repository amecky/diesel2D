#pragma once
#include "Converter.h"

namespace ds {

class ParticleSystemConverter : public Converter {

public:
	ParticleSystemConverter() : Converter("ParticleSystemConverter") {}
	virtual ~ParticleSystemConverter() {}
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\particles";
	}
};

}


