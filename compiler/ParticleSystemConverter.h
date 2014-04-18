#pragma once
#include "Converter.h"

namespace ds {

class ParticleSystemConverter : public Converter {

public:
	ParticleSystemConverter() : Converter("ParticleSystemConverter") {}
	virtual ~ParticleSystemConverter() {}
	void convert(const char* fileName);
};

}


