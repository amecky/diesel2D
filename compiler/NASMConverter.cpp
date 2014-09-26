#include "NASMConverter.h"
#include "..\script\ASM.h"

namespace ds {

	void NASMConverter::convert(const char* fileName) {
		char buffer[256];
		sprintf(buffer, "%s\\%s.nasm", getResourceDirectory(), fileName);
		LOG << "loading file: " << buffer;
		char out[256];
		IdString dataName = string::murmur_hash(buffer);
		sprintf(out, "data\\%u", dataName);
		LOG << "writing to: " << out;

		nasm::ASMScript ctx;
		if (nasm::loadScript(buffer, &ctx)) {
			nasm::debug(ctx);
			BinaryWriter writer;
			int signature[] = { 0, 8, 15 };
			writer.open(out, signature, 3);

			// save data
			writer.startChunk(1, 1);
			writer.write(ctx.numVars);
			for (int i = 0; i < ctx.numVars; ++i){
				writer.write(ctx.varHash[i]);
				writer.write(ctx.varData[i]);
			}
			writer.closeChunk();

			// save methods
			writer.startChunk(2, 1);
			writer.write(ctx.numMethods);
			int total = ctx.numMethods * 3;
			for (int i = 0; i < total; ++i) {
				writer.write(ctx.methodTable[i]);
			}
			writer.closeChunk();

			// save opcodes
			writer.startChunk(3, 1);
			writer.write(ctx.numOperations);
			total = ctx.numOperations * 14;
			for (int i = 0; i < total; ++i) {
				writer.write(ctx.operations[i]);
			}
			writer.closeChunk();
			writer.close();
		}
	}

}