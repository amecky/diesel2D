#pragma once
#include "Converter.h"

namespace ds {

	class PathConverter : public Converter {

	public:
		PathConverter() : Converter("PathConverter") {}
		~PathConverter() {}

		void convert(JSONReader& reader, BinaryWriter& writer);

		const char* getResourceDirectory() {
			return "content\\resources";
		}

	};
	
}

