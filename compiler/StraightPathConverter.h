#pragma once
#include "Converter.h"

namespace ds {

	class StraightPathConverter : public Converter {

	public:
		StraightPathConverter() : Converter("StraightPathConverter") {}
		~StraightPathConverter() {}

		void convert(JSONReader& reader, BinaryWriter& writer);

		const char* getResourceDirectory() {
			return "content\\resources";
		}

	};

	class StraightPath3DConverter : public Converter {

	public:
		StraightPath3DConverter() : Converter("StraightPath3DConverter") {}
		~StraightPath3DConverter() {}

		void convert(JSONReader& reader, BinaryWriter& writer);

		const char* getResourceDirectory() {
			return "content\\resources";
		}

	};

}

