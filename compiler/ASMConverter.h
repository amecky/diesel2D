#pragma once
#include "Converter.h"

namespace ds {

	class ASMConverter : public Converter {

	public:
		ASMConverter() : Converter("ASMConverter") {}
		~ASMConverter() {}

		virtual void convert(const char* fileName);

		void convert(JSONReader& reader, BinaryWriter& writer) {}

		const char* getResourceDirectory() {
			return "content\\asm";
		}

		virtual const char* getEnding() const {
			return "asm";
		}

	};

}

