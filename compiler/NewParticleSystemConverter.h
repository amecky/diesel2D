#pragma once
#include "Converter.h"

namespace ds {

class NewParticleSystemConverter : public Converter {

public:
	NewParticleSystemConverter() : Converter("NewParticleSystemConverter") {}
	virtual ~NewParticleSystemConverter() {}
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\particles";
	}
};

}