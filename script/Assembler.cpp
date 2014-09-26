/*
* File:   Assembler.cpp
* Author: meckya
*
* Created on 12. August 2014, 07:34
*/
#include "Assembler.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>
#include <assert.h>
#include "..\math\GameMath.h"
#include "..\utils\Log.h"
#include "..\particles\Particle.h"
#include "..\utils\Profiler.h"

namespace as {

	const int NUM_OPERATIONS = 41;
	
	enum ByteCode {
		OP_LOD,
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_CMP,
		OP_BEQ,
		OP_BNE,
		OP_JMP,
		OP_LRP,
		OP_LDA,
		OP_STC,
		OP_LDX,
		OP_INX,
		OP_DEX,
		OP_CPX,
		OP_STA,
		OP_RND,
		OP_SIN,
		OP_COS,
		OP_RAE,
		OP_BCS,
		OP_BCC,
		OP_PRN,
		OP_DBG,
		OP_ADX,
		OP_SUX,
		OP_DIX,
		OP_MUX,
		OP_LDY,
		OP_INY,
		OP_DEY,		
		OP_CPY,
		OP_STP,
		OP_DBP,
		OP_STY,
		OP_LDP,
		OP_FND,
		OP_STX,
		OP_SMS,
		OP_CLM
	};

	struct OpDef {
		const char* name;
		int paramLength;
		ByteCode byteCode;
	};

	static OpDef DEFINITIONS[] = {
		{ "LOD", 2, OP_LOD },
		{ "ADD", 2, OP_ADD },
		{ "SUB", 2, OP_SUB },
		{ "MUL", 2, OP_MUL },
		{ "DIV", 2, OP_DIV },
		{ "CMP", 2, OP_CMP },
		{ "BEQ", 1, OP_BEQ },
		{ "BNE", 1, OP_BNE },
		{ "JMP", 1, OP_JMP },
		{ "LRP", 2, OP_LRP },
		{ "LDA", 1, OP_LDA },
		{ "STC", 2, OP_STC },
		{ "LDX", 1, OP_LDX },
		{ "INX", 0, OP_INX },
		{ "DEX", 0, OP_DEX },
		{ "CPX", 1, OP_CPX },
		{ "STA", 1, OP_STA },
		{ "RND", 2, OP_RND },
		{ "SIN", 2, OP_SIN },
		{ "COS", 2, OP_COS },
		{ "RAE", 1, OP_RAE },
		{ "BCS", 1, OP_BCS },
		{ "BCC", 1, OP_BCC },
		{ "PRN", 1, OP_PRN },
		{ "DBG", 0, OP_DBG },
		{ "ADX", 1, OP_ADX },
		{ "SUX", 1, OP_SUX },
		{ "DIX", 1, OP_DIX },
		{ "MUX", 1, OP_MUX },
		{ "LDY", 1, OP_LDY },
		{ "INY", 0, OP_INY },
		{ "DEY", 0, OP_DEY },
		{ "CPY", 1, OP_CPY },
		{ "STP", 2, OP_STP },
		{ "DBP", 0, OP_DBP },
		{ "STY", 1, OP_STY },
		{ "LDP", 2, OP_LDP },
		{ "FND", 2, OP_FND },
		{ "STX", 1, OP_STX },
		{ "SMS", 2, OP_SMS },
		{ "CLM", 1, OP_CLM }
	};

	namespace str {

		bool isCharacter(const char p) {
			if (p >= 'a' && p <= 'z') {
				return true;
			}
			else if (p >= 'A' && p <= 'Z') {
				return true;
			}
			if (p == '_') {
				return true;
			}
			return false;
		}

		bool isUpperCaseCharacter(const char p) {
			if (p >= 'A' && p <= 'Z') {
				return true;
			}
			return false;
		}

		bool isWhitespace(const char c) {
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
				return true;
			}
			return false;
		}

		bool isDigit(const char c) {
			return ((c >= '0' && c <= '9') || c == '-' || c == '+');
		}

		float strtof(const char* p, char** endPtr) {
			while (isWhitespace(*p)) {
				++p;
			}
			float sign = 1.0f;
			if (*p == '-') {
				sign = -1.0f;
				++p;
			}
			else if (*p == '+') {
				++p;
			}
			float value = 0.0f;
			while (isDigit(*p)) {
				value *= 10.0f;
				value = value + (*p - '0');
				++p;
			}
			if (*p == '.') {
				++p;
				float dec = 1.0f;
				float frac = 0.0f;
				while (isDigit(*p)) {
					frac *= 10.0f;
					frac = frac + (*p - '0');
					dec *= 10.0f;
					++p;
				}
				value = value + (frac / dec);
			}
			if (endPtr) {
				*endPtr = (char *)(p);
			}
			return value * sign;
		}

		int hash(const char *str, int length) {

			int h = 0;
			for (int i = 0; i < length; ++i) {
				h = h << 1 ^ *str++;
			}
			return h;
		}

		int readHash(const char* p, char** endPtr) {
			while (isWhitespace(*p)) {
				++p;
			}
			if (*p == '#' || *p == '+') {
				++p;
			}
			const char* ident = p;
			while (str::isCharacter(*p)) {
				++p;
			}
			char buf[32];
			int d = p - ident;
			for (int i = 0; i < d; ++i) {
				buf[i] = ident[i];
			}
			buf[d] = '\0';
			int hash = ds::string::murmur_hash(buf);
			if (endPtr) {
				*endPtr = (char *)(p);
			}
			return hash;
		}


	}

	// -------------------------------------------------------
	// op execution helper
	// -------------------------------------------------------
	struct OpExecution {
		float* source;
		float* dest;
		int sourceLength;
		int destinationLength;
	};

	// -------------------------------------------------------
	// determine address mode
	// -------------------------------------------------------
	int determineMode(const char p) {
		switch (p) {
		case 'r': return AM_REGISTER;
			break;
		case 'd': return AM_DATA;
			break;
		case 'b': return AM_BANK;
			break;
		case 'c': return AM_CHANNEL;
			break;
		case 'x': return AM_DATA_OFFSET;
			break;
		}
		return AM_DIRECT;
	}

	// -------------------------------------------------------
	// get length of parameter
	// -------------------------------------------------------
	int determinLength(int mode) {
		switch (mode) {
		case AM_DIRECT: return 1;
			break;
		case AM_DATA: return 4;
			break;
		case AM_REGISTER: return 1;
			break;
		case AM_BANK: return 4;
			break;
		case AM_CHANNEL: return 4;
			break;
		case AM_DATA_OFFSET: return 4;
			break;
		}
		return 1;
	}

	// -------------------------------------------------------
	// get type
	// -------------------------------------------------------
	int getArrayIndex(const char* text, int length) {
		for (int i = 0; i < NUM_OPERATIONS; ++i) {
			if (strncmp(DEFINITIONS[i].name, text, length) == 0) {
				return i;
			}
		}
		return -1;
	}

	struct ConstDef {
		const char* name;
		float value;
	};

	static ConstDef CONST_DEFINITIONS[] = {
		{ "PI", ds::PI },
		{ "TWO_PI", TWO_PI }
	};

	const int NUM_CONSTS = 2;

	bool findConst(const char* p, char** endPtr, float* v) {
		const char* ident = p;
		while (str::isCharacter(*p)) {
			++p;
		}
		char buf[32];
		int d = p - ident;
		for (int i = 0; i < d; ++i) {
			buf[i] = ident[i];
		}
		buf[d] = '\0';
		bool found = false;
		for (int i = 0; i < NUM_CONSTS; ++i) {
			if (strcmp(CONST_DEFINITIONS[i].name, buf) == 0) {
				*v = CONST_DEFINITIONS[i].value;
				found = true;
			}
		}

		if (endPtr) {
			*endPtr = (char *)(p);
		}
		return found;
	}

	struct ScriptLabel {
		int hash;
		int line;
	};

	struct LabelRef {
		int hash;
		int idx;
	};
	// -------------------------------------------------------
	// Parse code and convert it to op codes
	// -------------------------------------------------------
	void parse(const char* p, AssemblerScript* script) {
		int cnt = 0;
		int dataIndex = 0;
		int line = 0;
		ScriptLabel labels[32];
		int numLabels = 0;
		LabelRef references[128];
		int numRefs = 0;
		while (*p != 0) {
			if (str::isCharacter(*p)) {
				const char* ident = p;
				while (str::isCharacter(*p)) {
					++p;
				}
				int arrayIndex = getArrayIndex(ident, p - ident);
				int pc = DEFINITIONS[arrayIndex].paramLength;
				OPCode& oc = script->operations[cnt++];
				oc.type = arrayIndex;
				if (pc > 0) {
					++p;
					// might be a label
					if (*p == '+') {
						char* out;
						int hash = str::readHash(p, &out);
						LabelRef& r = references[numRefs++];
						r.hash = hash;
						r.idx = cnt;
						p = out;
					}
					else {
						while (str::isWhitespace(*p)) {
							++p;
						}
						oc.modes[0] = determineMode(*p);
						if (oc.modes[0] != AM_DIRECT) {
							++p;
						}
						if (str::isDigit(*p)) {
							char* out;
							float value = str::strtof(p, &out);
							p = out;
							oc.values[oc.numValues++] = value;
						}
					}
				}
				if (pc > 1) {
					++p;
					while (str::isWhitespace(*p)) {
						++p;
					}
					// might be a const
					bool isConst = false;
					if (str::isUpperCaseCharacter(*p)) {
						char* out;
						float v = 0.0f;
						if (findConst(p, &out, &v)) {
							isConst = true;
							oc.modes[1] = AM_DIRECT;
							oc.values[1] = v;
							p = out;
						}
					}
					if (!isConst) {
						oc.modes[1] = determineMode(*p);
						if (oc.modes[1] != AM_DIRECT) {
							++p;
						}
						if (str::isDigit(*p)) {
							char* out;
							float value = str::strtof(p, &out);
							p = out;
							oc.values[oc.numValues++] = value;
						}
					}
				}
				++line;
			}
			// label
			else if (*p == '#') {
				char* out;
				int hash = str::readHash(p, &out);
				ScriptLabel& lbl = labels[numLabels++];
				lbl.hash = hash;
				lbl.line = line;
				p = out;
				//++line;
			}
			else if ( *p == '{' ) {
				char* out;
				++p;
				while (*p != '}') {					
					script->context.data[dataIndex++] = str::strtof(p, &out);
					p = out;
					if (*p != '}') {
						++p;
					}
				}
			}
			else {
				++p;
			}
		}
		bool found = true;
		for (int i = 0; i < numRefs; ++i) {
			LabelRef r = references[i];
			found = false;
			for (int j = 0; j < numLabels; ++j) {
				if (labels[j].hash == r.hash) {
					int d = labels[j].line - r.idx;
					script->operations[r.idx - 1].values[0] = d;
					found = true;
				}
			}
			assert(found);
		}
		LOG << "loaded script: " << script->context.name;
		LOG << "operations: " << cnt;
		LOG << "labels: " << numLabels;
		LOG << "refrences: " << numRefs;
		script->numOperations = cnt;
	}

	// -------------------------------------------------------
	// load script
	// -------------------------------------------------------
	void loadScript(const char* fileName, AssemblerScript* script) {
		sprintf(script->context.name, "%s", fileName);
		std::string line;
		std::string result = "";
		std::ifstream myfile(fileName);
		if (myfile.is_open()) {
			while (std::getline(myfile, line)) {
				if (line.find(";") == std::string::npos) {
					result += line;
					result += "\n";
				}
			}
			myfile.close();
		}
		parse(result.c_str(), script);
	}
	
	// -------------------------------------------------------
	// get pointer to specific data
	// -------------------------------------------------------
	float* getPtr(AssemblerContext& context, OPCode& opCode, int paramIndex) {
		if (opCode.modes[paramIndex] == AM_DIRECT) {
			return &opCode.values[paramIndex];
		}
		else if (opCode.modes[paramIndex] == AM_DATA) {
			int idx = (int)opCode.values[paramIndex];
			return &context.data[idx];
		}
		else if (opCode.modes[paramIndex] == AM_REGISTER) {
			int idx = (int)opCode.values[paramIndex];
			return &context.registers[idx];
		}
		else if (opCode.modes[paramIndex] == AM_BANK) {
			int idx = (int)opCode.values[paramIndex];
			return &context.registers[idx * 4];
		}
		else if (opCode.modes[paramIndex] == AM_DATA_OFFSET) {
			int idx = opCode.values[paramIndex];
			idx += context.x;
			return &context.data[idx];
		}
		else if (opCode.modes[paramIndex] == AM_CHANNEL) {
			int idx = (int)opCode.values[paramIndex];
			assert(idx < context.variables.num);
			int offset = context.variables.indices[idx];
			return context.variables.data[offset];
		}
		return 0;
	}


	// -------------------------------------------------------
	// copy data
	// -------------------------------------------------------
	void loadData(AssemblerContext& context, OPCode& opCode, const OpExecution& exec) {
		int s = 0;
		if (exec.dest != 0 && opCode.modes[1] == AM_CHANNEL) {
			int idx = (int)opCode.values[1];
			int offset = context.variables.indices[idx];
			int cnt = context.variables.count[idx];
			for (int i = 0; i < cnt; ++i) {
				exec.dest[i] = *context.variables.data[offset + i];
			}
		}
		for (int i = 0; i < exec.destinationLength; ++i) {
			exec.dest[i] = exec.source[s];
			if (s < exec.sourceLength) {
				++s;
			}
		}
	}

	// -------------------------------------------------------
	// store variable
	// -------------------------------------------------------
	void storeVariable(AssemblerContext& context, OPCode& opCode, const OpExecution& exec) {
		assert(opCode.modes[0] == AM_CHANNEL);
		if (exec.source != 0 && opCode.modes[0] == AM_CHANNEL) {
			int idx = opCode.values[0];
			int offset = context.variables.indices[idx];
			int cnt = context.variables.count[idx];
			for (int i = 0; i < cnt; ++i) {
				*context.variables.data[offset + i] = exec.source[i];
			}			
		}		
	}

	// -------------------------------------------------------
	// compare data
	// -------------------------------------------------------
	void compareData(AssemblerContext& context, OPCode& opCode, const OpExecution& exec) {
		int s = 0;
		context.zeroFlag = false;
		context.carryFlag = false;
		for (int i = 0; i < exec.destinationLength; ++i) {
			float diff = exec.dest[i] - exec.source[s];
			if (diff >= 0.0f) {
				context.carryFlag = true;
			}
			if (diff == 0.0f) {
				context.zeroFlag = true;
			}
			if (s < exec.sourceLength) {
				++s;
			}
		}
	}

	// -------------------------------------------------------
	// compare data
	// -------------------------------------------------------
	void compareX(AssemblerContext& context, OPCode& opCode, const OpExecution& exec) {
		int s = 0;
		context.zeroFlag = false;
		context.carryFlag = false;
		float diff = context.x - exec.dest[0];
		if (diff >= 0.0f) {
			context.carryFlag = true;
		}
		if (diff == 0.0f) {
			context.zeroFlag = true;
		}
	}

	void compareY(AssemblerContext& context, OPCode& opCode, const OpExecution& exec) {
		int s = 0;
		context.zeroFlag = false;
		context.carryFlag = false;
		float diff = context.y - exec.dest[0];
		if (diff >= 0.0f) {
			context.carryFlag = true;
		}
		if (diff == 0.0f) {
			context.zeroFlag = true;
		}
	}

	void findData(AssemblerContext& context, OPCode& opCode, const OpExecution& exec) {
		if (opCode.modes[0] == AM_DATA) {
			int idx = opCode.values[0];
			float v = exec.source[0];
			bool done = false;
			int pos = 0;
			while (!done) {
				if (context.data[idx] >= v) {
					context.x = pos;
					done = true;
				}
				++pos;
				++idx;
				if (idx >= 255) {
					done = true;
				}
			}
		}
	}

	// -------------------------------------------------------
	// prepare OpExcecution
	// -------------------------------------------------------
	void prepareExecution(AssemblerContext& context, OPCode& opCode, OpExecution& exec) {
		int params = DEFINITIONS[opCode.type].paramLength;
		exec.source = getPtr(context, opCode, 1);
		exec.dest = getPtr(context, opCode, 0);
		exec.sourceLength = determinLength(opCode.modes[1]) - 1;
		exec.destinationLength = determinLength(opCode.modes[0]);
		if (params > 0) {
			assert(exec.dest != 0);
		}
		if (params > 1) {
			assert(exec.source != 0);
		}
	}

	// -------------------------------------------------------
	// Execute
	// -------------------------------------------------------
	void execute(AssemblerContext& context, OPCode* codes, int numCodes) {
		context.clearRegisters();
		context.event = -1;
		OpExecution execution;
		int pCounter = 0;
		while (pCounter < numCodes) {
			OPCode& oc = codes[pCounter];
			if ( oc.type == OP_LOD ) {
				prepareExecution(context, oc, execution);
				loadData(context, oc, execution);
			}
			else if (oc.type == OP_STC ) {
				prepareExecution(context, oc, execution);
				storeVariable(context, oc, execution);					
			}
			else if (oc.type == OP_STY) {
				prepareExecution(context, oc, execution);
				execution.dest[0] = context.y;
			}
			else if (oc.type == OP_STX) {
				prepareExecution(context, oc, execution);
				execution.dest[0] = context.x;
			}
			else if (oc.type == OP_STA) {
				prepareExecution(context, oc, execution);
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] = context.accumulator;
				}
			}
			else if (oc.type == OP_LDX) {
				prepareExecution(context, oc, execution);
				context.x = (int)execution.dest[0];
			}
			else if (oc.type == OP_LDY) {
				prepareExecution(context, oc, execution);
				context.y = (int)execution.dest[0];
			}
			else if (oc.type == OP_INX) {
				++context.x;
			}
			else if (oc.type == OP_DEX) {
				--context.x;
				context.zeroFlag = false;
				if (context.x <= 0) {
					context.zeroFlag = true;
				}
			}			
			else if (oc.type == OP_INY) {
				++context.y;
			}
			else if (oc.type == OP_DEY) {
				--context.y;
				context.zeroFlag = false;
				if (context.y <= 0) {
					context.zeroFlag = true;
				}
			}
			else if (oc.type == OP_LDA) {
				prepareExecution(context, oc, execution);
				context.accumulator = execution.dest[0];
			}
			else if (oc.type == OP_ADD) {
				prepareExecution(context, oc, execution);
				int s = 0;
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] += execution.source[s];
					if (s < execution.sourceLength) {
						++s;
					}
				}
			}
			else if (oc.type == OP_SUB) {
				prepareExecution(context, oc, execution);
				int s = 0;
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] -= execution.source[s];
					if (s < execution.sourceLength) {
						++s;
					}
				}
			}
			else if (oc.type == OP_MUL) {
				prepareExecution(context, oc, execution);
				int s = 0;
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] *= execution.source[s];
					if (s < execution.sourceLength) {
						++s;
					}
				}
			}
			else if (oc.type == OP_DIV) {
				prepareExecution(context, oc, execution);
				int s = 0;
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] /= execution.source[s];
					if (s < execution.sourceLength) {
						++s;
					}
				}
			}
			else if (oc.type == OP_CMP) {
				prepareExecution(context, oc, execution);
				compareData(context, oc, execution);
			}
			else if (oc.type == OP_LRP) {
				prepareExecution(context, oc, execution);
				float t = context.accumulator;
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] = execution.dest[i] + (execution.source[i] - execution.dest[i]) * t;
				}
			}
			else if (oc.type == OP_SMS) {
				prepareExecution(context, oc, execution);
				float t = context.accumulator;
				float st = t * t * (3.0f - 2.0f * t);
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] = execution.dest[i] + (execution.source[i] - execution.dest[i]) * st;
				}
			}
			else if (oc.type == OP_BEQ) {
				int jmp = (int)oc.values[0];
				if (context.zeroFlag) {
					pCounter += jmp;
				}
			}
			else if (oc.type == OP_BCS) {
				int jmp = (int)oc.values[0];
				if (context.carryFlag) {
					pCounter += jmp;
				}
			}
			else if (oc.type == OP_BCC) {
				int jmp = (int)oc.values[0];
				if (!context.carryFlag) {
					pCounter += jmp;
				}
			}
			else if (oc.type == OP_BNE) {
				int jmp = (int)oc.values[0];
				if (!context.zeroFlag) {
					pCounter += jmp;
				}
			}
			else if (oc.type == OP_JMP) {
				int jmp = (int)oc.values[0];
				pCounter += jmp;
			}
			else if (oc.type == OP_CPX) {
				prepareExecution(context, oc, execution);
				compareX(context, oc, execution);				
			}
			else if (oc.type == OP_CPY) {
				prepareExecution(context, oc, execution);
				compareY(context, oc, execution);
			}
			else if (oc.type == OP_RND) {
				prepareExecution(context, oc, execution);
				float min = execution.dest[0];
				float max = execution.source[0];
				context.accumulator = ds::math::random(min, max);
			}
			else if (oc.type == OP_SIN) {
				prepareExecution(context, oc, execution);
				float angle = execution.source[0];
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] = ds::math::fastSin(angle);
				}
			}
			else if (oc.type == OP_COS) {
				prepareExecution(context, oc, execution);
				float angle = execution.source[0];
				for (int i = 0; i < execution.destinationLength; ++i) {
					execution.dest[i] = ds::math::fastCos(angle);
				}
			}
			else if (oc.type == OP_RAE) {
				prepareExecution(context, oc, execution);
				context.event = execution.dest[0];
			}
			else if (oc.type == OP_PRN) {
				prepareExecution(context, oc, execution);
				std::string str = "";
				char buf[32];
				for (int i = 0; i < execution.destinationLength; ++i) {
					sprintf(buf, "%3.2f ", execution.dest[i]);
					str += buf;
				}
				LOG << str;
			}
			else if (oc.type == OP_DBG) {
				debug(context);
			}
			else if (oc.type == OP_ADX ) {
				prepareExecution(context, oc, execution);
				context.accumulator += execution.dest[0];
			}
			else if (oc.type == OP_SUX) {
				prepareExecution(context, oc, execution);
				context.accumulator -= execution.dest[0];
			}
			else if (oc.type == OP_DIX) {
				prepareExecution(context, oc, execution);
				context.accumulator /= execution.dest[0];
			}
			else if (oc.type == OP_MUX) {
				prepareExecution(context, oc, execution);
				context.accumulator *= execution.dest[0];
			}
			else if (oc.type == OP_STP) {
				prepareExecution(context, oc, execution);
				int offset = context.x * SIMPLE_PARTICLE_SIZE;
				int po = execution.dest[0];
				context.particleArray->data[offset + po] = execution.source[0];
			}
			else if (oc.type == OP_LDP) {
				prepareExecution(context, oc, execution);
				int offset = context.x * SIMPLE_PARTICLE_SIZE;
				int po = execution.source[0];
				execution.dest[0] = context.particleArray->data[offset + po];
			}
			else if (oc.type == OP_FND) {
				prepareExecution(context, oc, execution);
				findData(context, oc,execution);
			}
			else if (oc.type == OP_DBP) {
				prepareExecution(context, oc, execution);
				int offset = context.x * SIMPLE_PARTICLE_SIZE;
				std::string str = "";
				char buf[32];
				for (int i = 0; i < SIMPLE_PARTICLE_SIZE; ++i) {
					sprintf(buf, "%3.2f ", context.particleArray->data[offset + i]);
					str += buf;
				}
				LOG << str;
			}
			else if (oc.type == OP_CLM) {
				prepareExecution(context, oc, execution);
				for (int i = 0; i < execution.destinationLength; ++i) {
					if (execution.dest[i] < 0.0f) {
						execution.dest[i] = 0.0f;
					}
					else if (execution.dest[i] > 1.0f) {
						execution.dest[i] = 1.0f;
					}
				}
			}
			++pCounter;
		}
	}

	void execute(AssemblerScript& script) {
		execute(script.context, script.operations, script.numOperations);
	}

	// -------------------------------------------------------
	// Debug registers
	// -------------------------------------------------------

	void debug(const AssemblerContext& context) {
		LOG << "acc        : " << context.accumulator;
		LOG << "x          : " << context.x;
		LOG << "y          : " << context.y;
		LOG << "zero flag  : " << context.zeroFlag;
		LOG << "carry flag : " << context.carryFlag;
		for (int i = 0; i < 16; ++i) {
			LOG << i << " = " << context.registers[i];
		}
		//printf("data:\n");
		//for (int i = 0; i < 32; ++i) {
			//printf("%3.2f ", context.data[i]);
		//}
		//printf("\n");
	}

	void registerVar(AssemblerScript& script, Vector2f* ev) {
		Variables& c = script.context.variables;
		int idx = c.current;
		assert(idx >= 0 && idx < 32);
		c.data[idx] = &ev->x;
		c.data[idx + 1] = &ev->y;
		c.count[c.num] = 2;
		c.indices[c.num++] = c.current;
		c.current += 2;
	}

	void registerVar(AssemblerScript& script, ds::Color* ev) {
		Variables& c = script.context.variables;
		int idx = c.current;
		c.data[idx] = &ev->r;
		c.data[idx + 1] = &ev->g;
		c.data[idx + 2] = &ev->b;
		c.data[idx + 3] = &ev->a;
		c.count[c.num] = 4;
		c.indices[c.num++] = c.current;
		c.current += 4;
	}

	void registerVar(AssemblerScript& script, float* ev) {
		Variables& c = script.context.variables;
		int idx = c.current;
		c.data[idx] = ev;
		c.count[c.num] = 1;
		c.indices[c.num++] = c.current;
		c.current += 1;
	}

	// -------------------------------------------------------
	// set data
	// -------------------------------------------------------

	void setData(AssemblerScript& script, int offset, float* data, int num) {
		for (int i = 0; i < num; ++i) {
			script.context.data[offset + i] = data[i];
		}
	}

	void reset(AssemblerScript& script) {
		script.context.variables.num = 0;
		script.context.variables.current = 0;
		script.context.event = -1;
	}
}
