#include "ASMConverter.h"
#include "..\script\Assembler.h"

namespace ds {

	void ASMConverter::convert(const char* fileName) {
		char buffer[256];
		sprintf(buffer, "%s\\%s.asm", getResourceDirectory(), fileName);
		LOG << "loading file: " << buffer;
		char out[256];
		IdString dataName = string::murmur_hash(buffer);
		sprintf(out, "data\\%u", dataName);
		LOG << "writing to: " << out;

		as::AssemblerScript script;
		as::loadScript(buffer, &script);
		LOG << "op codes " << script.numOperations;
		BinaryWriter writer;
		int signature[] = { 0, 8, 15 };
		writer.open(out, signature, 3);

		// save data
		writer.startChunk(1, 1);
		for (int i = 0; i < as::MAX_DATA; ++i){
			writer.write(script.context.data[i]);
		}
		writer.closeChunk();

		// save opcodes
		writer.startChunk(2, 1);
		writer.write(script.numOperations);
		for (int i = 0; i < script.numOperations; ++i) {
			writer.write(script.operations[i].type);
			writer.write(script.operations[i].values[0]);
			writer.write(script.operations[i].values[1]);
			writer.write(script.operations[i].modes[0]);
			writer.write(script.operations[i].modes[1]);
			writer.write(script.operations[i].numValues);			
		}
		writer.closeChunk();
		writer.close();
		
	}

}