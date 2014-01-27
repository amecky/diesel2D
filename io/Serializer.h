#pragma once
#include "BinaryLoader.h"
#include "BinaryWriter.h"

namespace ds {

class Serializer {

public:
	Serializer() {}
	virtual ~Serializer() {}
	// load from json file
	virtual void load(const char* fileName) {}
	// load from binary data
	virtual void loadBinary(BinaryLoader* loader) {}
	// save to binary data
	virtual void save(BinaryWriter* writer) {}
	// is file reloadable
	virtual const bool isReloadable() const {
		return true;
	}
	// reload
	virtual void reload(const char* fileName) {}
};

}

