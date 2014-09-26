/*
* File:   ASM.h
* Author: meckya
*
* Created on 2. September 2014, 08:37
*/

#ifndef ASM_H
#define	ASM_H
#include "Vector.h"
#include "..\particles\Particle.h"
#include "..\io\Serializer.h"

namespace nasm {

	const int ASM_MAX_STEPS = 32;
	const int ASM_MAX_OPERATIONS = 256;
	const int ASM_MAX_TEMP_CHANNELS = 8;
	const int ASM_MAX_VARIABLES = 16;

	struct ASMScript : public ds::Serializer {
		Vector4f tempVarData[ASM_MAX_TEMP_CHANNELS][ASM_MAX_STEPS];
		int varHash[ASM_MAX_VARIABLES];
		Vector4f varData[ASM_MAX_VARIABLES];
		int numVars;
		int operations[ASM_MAX_OPERATIONS * 14];
		int numOperations;

		int numMethods;
		// hash start length
		int methodTable[24];

		Vector4f counter;

		void clear() {
			counter = Vector4f(0.0f);
			for (int i = 0; i < ASM_MAX_TEMP_CHANNELS; ++i) {
				for (int j = 0; j < ASM_MAX_STEPS; ++j) {
					tempVarData[i][j] = Vector4f(0, 0, 0, 0);
				}
			}
		}
		void load(BinaryLoader* loader) {
			while (loader->openChunk() == 0) {
				if (loader->getChunkID() == 1) {
					loader->read(&numVars);
					for (int i = 0; i < numVars; ++i) {
						loader->read(&varHash[i]);
						loader->read(&varData[i]);
					}
					loader->closeChunk();
				}
				else if (loader->getChunkID() == 2) {
					loader->read(&numMethods);
					int total = numMethods * 3;
					for (int i = 0; i < total; ++i) {
						loader->read(&methodTable[i]);
					}
					loader->closeChunk();
				}
				else if (loader->getChunkID() == 3) {
					loader->read(&numOperations);
					int total = numOperations * 14;
					for (int i = 0; i < total; ++i) {
						loader->read(&operations[i]);
					}
					loader->closeChunk();
				}
			}
		}
	};

	bool loadScript(const char* fileName, ASMScript* ctx);

	void debug(const ASMScript& ctx);

	void debugTempData(const ASMScript& ctx, int index);

	void execute(ASMScript& ctx, const char* method, NParticleArray& array);

	void emitt(ASMScript& ctx, const char* methodName, float x, float y, NParticleArray& array);

	void patch_constant(ASMScript& ctx, const char* name, float value);

	void patch_constant(ASMScript& ctx, const char* name, const Vector4f& value);

	void patch_constant(ASMScript& ctx, int hash, const Vector4f& value);

}


#endif	/* ASM_H */
