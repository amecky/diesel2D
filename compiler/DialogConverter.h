#pragma once
#include "Converter.h"

namespace ds {

class DialogConverter : public Converter {

public:
	DialogConverter() : Converter("DialogConverter") {}
	~DialogConverter() {}
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\dialogs";
	}
};

}

