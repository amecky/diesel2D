#pragma once
#include "BinaryLoader.h"
#include "BinaryWriter.h"

namespace ds {

class Serializer {

public:
	Serializer() {}
	virtual ~Serializer() {}
	// load from binary data
	virtual void load(BinaryLoader* loader) = 0;
	
};

}

