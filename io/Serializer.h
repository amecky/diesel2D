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

class BinarySerializer {

public:
	BinarySerializer() {}
	virtual ~BinarySerializer() {}
	// load from binary data
	virtual int load(const char* fileName) = 0;
	virtual const char* getResourceDirectory()  = 0;
};

}

