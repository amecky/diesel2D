/*
* File:   Assembler.h
* Author: meckya
*
* Created on 12. August 2014, 07:34
*/

#ifndef ASSEMBLER_H
#define	ASSEMBLER_H
#pragma once
#include <vector>
#include "Vector.h"
#include "..\io\Serializer.h"
#include "..\utils\Color.h"

struct SimpleParticleArray;

namespace as {

	const int MAX_OPERATIONS = 128;
	const int MAX_DATA = 128;
	// -------------------------------------------------------
	// address mode
	// -------------------------------------------------------

	/*
	# - direct no prefix
	# - register - r prefix
	# - data - d prefix
	# - bank - b prefix
	# - channel - c prefix
	# - data offset - x prefix
	*/
	const int AM_DIRECT      = 0;
	const int AM_REGISTER    = 1;
	const int AM_DATA        = 2;
	const int AM_BANK        = 3;
	const int AM_CHANNEL     = 4;
	const int AM_DATA_OFFSET = 5;

	// -------------------------------------------------------
	// OP Code
	// -------------------------------------------------------

	struct OPCode {

		OPCode() : type(-1), numValues(0) {
			values[0] = 0.0f;
			values[1] = 0.0f;
			modes[0] = AM_DIRECT;
			modes[1] = AM_DIRECT;
		}

		OPCode(int type) : type(type), numValues(0) {
			values[0] = 0.0f;
			values[1] = 0.0f;
			modes[0] = AM_DIRECT;
			modes[1] = AM_DIRECT;
		}

		int type;
		float values[2];
		int modes[2];
		int numValues;


	};

	struct Variables {

		float* data[32];
		int indices[8];
		int num;
		int current;
		int count[8];
	};

	// -------------------------------------------------------
	// Assembler context
	// -------------------------------------------------------	
	struct AssemblerContext {

		char name[32];
		float data[MAX_DATA];
		float registers[16];
		float accumulator;
		bool carryFlag;
		bool zeroFlag;
		int x;
		int y;
		int event;
		Variables variables;
		SimpleParticleArray* particleArray;

		void clearRegisters() {
			for (int i = 0; i < 16; ++i) {
				registers[i] = 0.0f;
			}
			accumulator = 0.0f;
			zeroFlag = false;
			carryFlag = false;
			x = 0;
			y = 0;
		}
	};

	// -------------------------------------------------------
	// Assembler script
	// -------------------------------------------------------	
	struct AssemblerScript : public ds::Serializer {

		AssemblerContext context;
		OPCode operations[MAX_OPERATIONS];
		int numOperations;

		AssemblerScript() : numOperations(0) {
			context.variables.num = 0;
			for (int i = 0; i < MAX_DATA; ++i) {
				context.data[i] = 0.0f;
			}
		}

		void load(BinaryLoader* loader) {
			context.variables.num = 0;
			while (loader->openChunk() == 0) {
				if (loader->getChunkID() == 1) {
					for (int i = 0; i < MAX_DATA; ++i) {
						loader->read(&context.data[i]);
					}
					loader->closeChunk();
				}
				else if (loader->getChunkID() == 2) {
					int num = 0;
					loader->read(&num);
					for (int i = 0; i < num; ++i) {
						loader->read(&operations[i].type);
						loader->read(&operations[i].values[0]);
						loader->read(&operations[i].values[1]);
						loader->read(&operations[i].modes[0]);
						loader->read(&operations[i].modes[1]);
						loader->read(&operations[i].numValues);
					}
					numOperations = num;
					loader->closeChunk();
				}												
			}
		}
	};


	// -------------------------------------------------------
	// methods
	// -------------------------------------------------------

	void loadScript(const char* fileName, AssemblerScript* script);

	void parse(const char* p, AssemblerScript* script);

	void execute(AssemblerScript& script);

	void debug(const AssemblerContext& context);

	void setData(AssemblerScript& script, int offset, float* data, int num);

	void registerVar(AssemblerScript& script, Vector2f* ev);

	void registerVar(AssemblerScript& script, ds::Color* ev);

	void registerVar(AssemblerScript& script, float* ev);

	void reset(AssemblerScript& script);
}


#endif	/* ASSEMBLER_H */
