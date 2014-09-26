#pragma once
#include "Converter.h"

namespace ds {

	class NASMConverter : public Converter {

	public:
		NASMConverter() : Converter("NASMConverter") {}
		~NASMConverter() {}

		virtual void convert(const char* fileName);

		void convert(JSONReader& reader, BinaryWriter& writer) {}

		const char* getResourceDirectory() {
			return "content\\asm";
		}

		virtual const char* getEnding() const {
			return "nasm";
		}

	};

}

