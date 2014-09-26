#include "ASM.h"
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <string>

namespace str {

	// -------------------------------------------------------
	// build hash
	// -------------------------------------------------------
	int hash(const char *key) {
		int hash = 0;
		int len = strlen(key);
		for (int i = 0; i < len; ++i) {
			hash += key[i];
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}

	int hash(const char *key, int length) {
		int hash = 0;
		for (int i = 0; i < length; ++i) {
			hash += key[i];
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}

	// -------------------------------------------------------
	// check wether character is a digit
	// -------------------------------------------------------
	bool isDigit(const char c) {
		return ((c >= '0' && c <= '9') || c == '-' || c == '+');
	}

	// -------------------------------------------------------
	// check wether character is a char
	// -------------------------------------------------------
	bool isCharacter(const char p) {
		if (p >= 'a' && p <= 'z') {
			return true;
		}
		else if (p >= 'A' && p <= 'Z') {
			return true;
		}
		else if (p == '_') {
			return true;
		}
		return isDigit(p);
	}

	// -------------------------------------------------------
	// check wether it is an uppercase char
	// -------------------------------------------------------
	bool isUpperCaseCharacter(const char p) {
		if (p >= 'A' && p <= 'Z') {
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// check wether character is a whitespace char
	// -------------------------------------------------------
	bool isWhitespace(const char c) {
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// convert text to float
	// -------------------------------------------------------
	float strtof(const char* p, char** endPtr) {
		while (isWhitespace(*p)) {
			++p;
		}
		float sign = 1.0f;
		if (*p == '-') {
			sign = -1.0f;
			++p;
		}
		if (*p == '+') {
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

	// -------------------------------------------------------
	// read text and build hash
	// -------------------------------------------------------
	int readHash(const char* p, char** endPtr) {
		while (isWhitespace(*p)) {
			++p;
		}
		const char* ident = p;
		while (str::isCharacter(*p) || *p == '#' || *p == '+') {
			++p;
		}
		char buf[32];
		int d = p - ident;
		for (int i = 0; i < d; ++i) {
			buf[i] = ident[i];
		}
		buf[d + 1] = '\0';
		int hash = str::hash(buf);
		//printf("--> hash '%s' = %d length %d\n",buf,hash,d);
		if (endPtr) {
			*endPtr = (char *)(p);
		}
		return hash;
	}

	// -------------------------------------------------------
	// print helper
	// -------------------------------------------------------
	void print(const char* msg, const char* p, int len) {
		char b[64];
		for (int i = 0; i < len; ++i) {
			b[i] = p[i];
		}
		b[len] = '\0';
		LOG << msg << " " << b;;
	}

}

namespace nasm {

	// -------------------------------------------------------
	// 
	// -------------------------------------------------------
	struct ASMToken {

		enum ASMTokenType {
			UNKNOWN,
			VARIABLE,
			FLOAT,
			ASSIGN,
			NAME,
			FUNCTION,
			METHOD,
			CONSTANT,
			OPEN,
			CLOSE,
			STRUCT,
			CHANNEL,
			TEMP_VAR,
			COLON,
			SEMICOLON,
			DELIMITER,
			DOT,
			COUNTER,
			EOL
		};

		ASMToken() : type(UNKNOWN), id(0) {
		}

		ASMToken(ASMTokenType type) : type(type) {
		}

		ASMToken(ASMTokenType type, float value) : type(type), value(value) {
		}

		ASMToken(ASMTokenType type, int id) : type(type), id(id) {
		}

		ASMTokenType type;

		union {
			int id;
			float value;
		};

		char txt[32];
	};

	struct StructDefinition {
		int hash[8];
		int channels[8];
		int num;

	};

	enum ASMErrorCode {
		EC_OK,
		EC_METHOD_NOT_FOUND,
		EC_UNKNOWN_NAME,
		EC_INVALID_TOKEN
	};

	struct ASMParserContext {

		StructDefinition structs[2];
		int tempVars[16];
		Vector4f tempVarData[8][ASM_MAX_STEPS];
		int varHash[16];
		Vector4f varData[16];
		int numVars;
		int operations[14 * ASM_MAX_OPERATIONS];
		int numOperations;
		int numTemp;
		int numMethods;
		int methodTable[24];
		int opCounter;

		ASMErrorCode errorCode;
		char errorMessage[256];

		ASMParserContext() : numVars(0) {
			structs[0].num = 0;
			structs[1].num = 0;
			numTemp = 0;
			numOperations = 0;
			opCounter = 0;
			for (int i = 0; i < 14 * ASM_MAX_OPERATIONS; ++i) {
				operations[i] = 0;
			}
			errorCode = EC_OK;
		}

		int findVar(int hash) {
			for (int i = 0; i < numVars; ++i) {
				if (varHash[i] == hash) {
					return i;
				}
			}
			return -1;
		}

		int findTempVar(int hash) {
			for (int i = 0; i < numTemp; ++i) {
				if (tempVars[i] == hash) {
					return i;
				}
			}
			return -1;
		}

		int findStructVar(int hash, int idx) {
			StructDefinition& def = structs[idx];
			for (int i = 0; i < def.num; ++i) {
				if (def.hash[i] == hash) {
					return def.channels[i];
				}
			}
			return -1;
		}

		int createTemp(int hash) {
			int idx = numTemp;
			tempVars[idx] = hash;
			++numTemp;
			return idx;
		}
	};

	const char* translate(const ASMToken& token) {
		switch (token.type) {
		case ASMToken::UNKNOWN: return "UNKNOWN";
		case ASMToken::VARIABLE: return "VARIABLE";
		case ASMToken::FLOAT: return "FLOAT";
		case ASMToken::ASSIGN: return "ASSIGN";
		case ASMToken::NAME: return "NAME";
		case ASMToken::FUNCTION: return "FUNCTION";
		case ASMToken::CONSTANT: return "CONST";
		case ASMToken::OPEN: return "OPEN";
		case ASMToken::CLOSE: return "CLOSE";
		case ASMToken::STRUCT: return "STRUCT";
		case ASMToken::CHANNEL: return "CHANNEL";
		case ASMToken::TEMP_VAR: return "TEMP_VAR";
		case ASMToken::COLON: return "COLON";
		case ASMToken::SEMICOLON: return "SEMICOLON";
		case ASMToken::DOT: return "DOT";
		case ASMToken::METHOD: return "METHOD";
		case ASMToken::COUNTER: return "COUNTER";
		case ASMToken::EOL: return "EOL";
		}
		return "UNKNOWN";
	}

	// -------------------------------------------------
	// ASM operation definitions
	// -------------------------------------------------

	struct OpDef {
		const char* name;
		int paramLength;
	};

	const int NUM_OPERATIONS = 14;

	static OpDef DEFINITIONS[] = {
		{ "ADD", 2 },
		{ "SUB", 2 },
		{ "MUL", 2 },
		{ "DIV", 2 },
		{ "SIN", 1 },
		{ "MOX", 1 },
		{ "MOY", 1 },
		{ "MOZ", 1 },
		{ "RND", 2 },
		{ "ASG", 1 },
		{ "RAD", 1 },
		{ "LCN", 2 },
		{ "LCI", 3 },
		{ "SUV", 1 }
	};

	// -------------------------------------------------
	// Constant definitions
	// -------------------------------------------------
	const int NUM_CONSTANTS = 9;

	struct ConstantDef {
		const char* name;
		Vector4f value;
	};

	static ConstantDef CONSTANTS[] = {
		{ "PI", Vector4f(3.14159265358979323846f) },
		{ "TWOPI", Vector4f(2.0 * 3.14159265358979323846f) },
		{ "ONE", Vector4f(1.0f) },
		{ "ONEXY", Vector4f(1.0f, 1.0f, 0.0f, 0.0f) },
		{ "ZERO", Vector4f(0.0f) },
		{ "UP", Vector4f(0.0f, 1.0f, 0.0f, 0.0f) },
		{ "DOWN", Vector4f(0.0f, -1.0f, 0.0f, 0.0f) },
		{ "LEFT", Vector4f(-1.0f, 0.0f, 0.0f, 0.0f) },
		{ "RIGHT", Vector4f(1.0f, 0.0f, 0.0f, 0.0f) }
	};

	// -------------------------------------------------------
	// check if name is a function
	// -------------------------------------------------------
	bool isFunction(const char* p, int len) {
		if (len != 3) {
			return false;
		}
		for (int i = 0; i < NUM_OPERATIONS; ++i) {
			if (strncmp(DEFINITIONS[i].name, p, len) == 0) {
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// check if name is a known constant
	// -------------------------------------------------------
	bool isConstant(const char* p, int len) {
		for (int i = 0; i < NUM_CONSTANTS; ++i) {
			if (strncmp(CONSTANTS[i].name, p, len) == 0) {
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// get index of function
	// -------------------------------------------------------
	int getFunctionIndex(const char* text, int length) {
		for (int i = 0; i < NUM_OPERATIONS; ++i) {
			if (strncmp(DEFINITIONS[i].name, text, length) == 0) {
				return i;
			}
		}
		return -1;
	}

	// -------------------------------------------------------
	// get index of constant
	// -------------------------------------------------------
	int getConstantIndex(const char* text, int length) {
		for (int i = 0; i < NUM_CONSTANTS; ++i) {
			if (strncmp(CONSTANTS[i].name, text, length) == 0) {
				return i;
			}
		}
		return -1;
	}

	// -------------------------------------------------------
	// check if name is a temp var name and return index
	// -------------------------------------------------------
	int getTempVarIndex(const char* p, int length) {
		// t0 - t7
		if (length == 2 && *p == 't') {
			++p;
			if (str::isDigit(*p)) {
				int index = *p - '0';
				if (index >= 0 && index < 8) {
					return index;
				}
			}
		}
		return -1;
	}
	// -----------------------------------------------------------
	// Convert source code into token stream
	// -----------------------------------------------------------
	int parse(const char* p, ASMToken* tokens, int maxTokens) {
		int cnt = 0;
		while (*p != 0) {
			ASMToken t;
			// Numbers
			if (str::isDigit(*p)) {
				char* out;
				float value = str::strtof(p, &out);
				p = out;
				t = ASMToken(ASMToken::FLOAT, value);
			}
			else if (str::isCharacter(*p)) {
				t = ASMToken(ASMToken::NAME, 1.0f);
				const char* ident = p;
				while (str::isCharacter(*p)) {
					++p;
				}
				int index = getFunctionIndex(ident, p - ident);
				if (index != -1) {
					t = ASMToken(ASMToken::FUNCTION, index);
				}
				else if (isConstant(ident, p - ident)) {
					int constantIndex = getConstantIndex(ident, p - ident);
					t = ASMToken(ASMToken::CONSTANT, constantIndex);
				}
				else if (getTempVarIndex(ident, p - ident) != -1) {
					t = ASMToken(ASMToken::TEMP_VAR, getTempVarIndex(ident, p - ident));
				}
				else {
					if (strncmp("const", ident, p - ident) == 0) {
						t = ASMToken(ASMToken::VARIABLE);
					}
					else if (strncmp("struct", ident, p - ident) == 0) {
						t = ASMToken(ASMToken::STRUCT);
					}
					else if (strncmp("function", ident, p - ident) == 0) {
						t = ASMToken(ASMToken::METHOD);
					}
					else if (strncmp("COUNTER", ident, p - ident) == 0) {
						t = ASMToken(ASMToken::COUNTER);
					}
					else if (strncmp("CHANNEL", ident, p - ident - 1) == 0) {
						int value = *(p - 1) - '0';
						t = ASMToken(ASMToken::CHANNEL, value);
					}
					else {
						int hash = str::hash(ident, p - ident);
						t = ASMToken(ASMToken::NAME, hash);
						int d = p - ident;
						for (int i = 0; i < d; ++i) {
							t.txt[i] = ident[i];
						}
						t.txt[d] = '\0';
					}
				}
			}
			else {
				switch (*p) {
				case '{': t = ASMToken(ASMToken::OPEN);
					break;
				case '}': t = ASMToken(ASMToken::CLOSE);
					break;
				case ';': t = ASMToken(ASMToken::EOL);
					break;
				case '=': t = ASMToken(ASMToken::ASSIGN);
					break;
				case ':': t = ASMToken(ASMToken::COLON);
					break;
				case '.': t = ASMToken(ASMToken::DOT);
					break;
				case ' ':
				case '\t':
				case '\r':
				case '\n':
					break;
				default:
					break;
				}
				++p;
			}
			if (t.type != ASMToken::UNKNOWN && t.type != ASMToken::SEMICOLON) {
				if (cnt < maxTokens) {
					tokens[cnt++] = t;
				}
			}
		}
		return cnt;
	}

	// -----------------------------------------------------------
	// parse const definition
	// -----------------------------------------------------------
	int parse_const(ASMToken* tokens, int cnt, ASMParserContext* ctx) {
		// CONST NAME ASSIGN OPEN FLOAT FLOAT FLOAT FLOAT CLOSE
		int pos = cnt;
		ASMToken nt = tokens[pos++];
		if (nt.type == ASMToken::NAME) {
			if (tokens[pos++].type == ASMToken::ASSIGN) {
				if (tokens[pos++].type == ASMToken::OPEN) {
					int vidx = ctx->numVars++;
					ctx->varHash[vidx] = nt.id;
					for (int i = 0; i < 4; ++i) {
						ASMToken ft = tokens[pos++];
						if (ft.type == ASMToken::FLOAT) {
							ctx->varData[vidx].data[i] = ft.value;
						}
					}
				}
			}
		}
		return pos;
	}

	// ------------------------------------------------------
	// Convert struct token stream into bytecode
	// ------------------------------------------------------
	int parse_struct(ASMToken* tokens, int cnt, ASMParserContext* ctx) {
		// NAME OPEN NAME COLON CHANNEL EOL .... CLOSE
		int pos = cnt;
		ASMToken nt = tokens[pos++];
		if (nt.type == ASMToken::NAME) {
			int sidx = 0;
			if (str::hash("OUT", 3) == nt.id) {
				sidx = 1;
			}
			StructDefinition& def = ctx->structs[sidx];
			// OPEN
			if (tokens[pos++].type == ASMToken::OPEN) {
				ASMToken t = tokens[pos++];
				while (t.type != ASMToken::CLOSE) {
					if (t.type == ASMToken::NAME) {
						int hash = t.id;
						++pos;
						t = tokens[pos++];
						int channel = t.id;
						def.hash[def.num] = hash;
						def.channels[def.num] = channel;
						++def.num;
					}
					t = tokens[pos++];
				}
			}
			++pos; // CLOSE
		}
		return pos;
	}

	// ------------------------------------------------------
	// Build operation based on type and index and steps
	// ------------------------------------------------------
	void createOperation(ASMParserContext* ctx, int opIdx, int offset, int type, int index, int steps) {
		ctx->operations[opIdx + offset] = type;
		ctx->operations[opIdx + offset + 1] = index;
		ctx->operations[opIdx + offset + 2] = steps;
	}

	// ------------------------------------------------------
	// Determine operand 
	// ------------------------------------------------------
	int determine_operand(ASMToken* tokens, int cnt, ASMParserContext* ctx, int index, int offset) {
		ASMToken token = tokens[cnt];
		int pos = cnt;
		if (token.id == 1298113611) {
			// means IN
			pos += 2;
			ASMToken nt = tokens[pos++];
			if (nt.type == ASMToken::NAME) {
				createOperation(ctx, index, offset, 2, ctx->findStructVar(nt.id, 0), 1);
			}
		}
		else if (token.id == -1242890608) {
			// means OUT
			pos += 2;
			ASMToken nt = tokens[pos++];
			if (nt.type == ASMToken::NAME) {
				createOperation(ctx, index, offset, 3, ctx->findStructVar(nt.id, 1), 1);
			}
		}
		else if (token.type == ASMToken::COUNTER) {
			createOperation(ctx, index, offset, 4, 0, 0);
			++pos;
		}
		else if (token.type == ASMToken::CONSTANT) {
			createOperation(ctx, index, offset, 5, token.id, 0);
			++pos;
		}
		else if (token.type == ASMToken::TEMP_VAR) {
			createOperation(ctx, index, offset, 0, token.id, 1);
			++pos;
		}
		else if (ctx->findVar(token.id) != -1) {
			createOperation(ctx, index, offset, 1, ctx->findVar(token.id), 0);
			++pos;
		}
		else {
			ctx->errorCode = EC_INVALID_TOKEN;
			LOGE << "Cannot convert token " << token.txt;
			++pos;
		}
		return pos;
	}

	// ------------------------------------------------------
	// Parse operation token stream into bytecode
	// ------------------------------------------------------
	int parse_operation(ASMToken* tokens, int cnt, ASMParserContext* ctx) {
		// 0 = number of operands
		// 1 = function
		// 2 = result operand
		// 5 = operand 1
		// 8 = operand 2
		// 11 = operand 3
		// 14 in total
		// NAME ASSIGN FUNCTION NAME NAME (NAME) EOL
		int opc = ctx->numOperations++;
		int pos = cnt;
		int idx = opc * 14;
		ctx->operations[idx] = 0;
		pos = determine_operand(tokens, cnt, ctx, idx, 2);
		if (tokens[pos++].type == ASMToken::ASSIGN) {
			ASMToken ft = tokens[pos++];
			//LOG << "function " << translate(ft);
			ctx->operations[idx + 1] = ft.id;
			if (ft.type == ASMToken::NAME) {
				ctx->operations[idx + 1] = 9;
				pos = determine_operand(tokens, pos - 1, ctx, idx, 5);
			}
			else if (ft.type == ASMToken::COUNTER) {
				ctx->operations[idx + 1] = 9;
				pos = determine_operand(tokens, pos - 1, ctx, idx, 5);
			}
			else if (ft.type == ASMToken::CONSTANT) {
				ctx->operations[idx + 1] = 9;
				pos = determine_operand(tokens, pos - 1, ctx, idx, 5);
			}
			else if (ft.type == ASMToken::TEMP_VAR) {
				ctx->operations[idx + 1] = 9;
				pos = determine_operand(tokens, pos - 1, ctx, idx, 5);
			}
			ASMToken ot = tokens[pos++];
			int count = 0;
			while (ot.type != ASMToken::EOL) {
				pos = determine_operand(tokens, pos - 1, ctx, idx, count * 3 + 5);
				ot = tokens[pos++];
				++count;
			}
		}
		else {
			LOGE << "MISSING ASSIGNMENT!!!";
		}
		return pos;
	}

	// ------------------------------------------------------
	// Parse method token stream
	// ------------------------------------------------------
	int parse_method(ASMToken* tokens, int cnt, ASMParserContext* ctx) {
		int pos = cnt;
		ASMToken token = tokens[pos];
		if (token.type == ASMToken::NAME) {
			int methodHash = token.id;
			int methodIndex = ctx->numMethods++;
			ctx->methodTable[methodIndex * 3] = methodHash;
			ctx->methodTable[methodIndex * 3 + 1] = ctx->opCounter;
			ctx->methodTable[methodIndex * 3 + 2] = 0;
			pos += 2;
			ASMToken opt = tokens[pos];
			while (opt.type != ASMToken::CLOSE) {
				pos = parse_operation(tokens, pos, ctx);
				++ctx->opCounter;
				opt = tokens[pos];
			}
			ctx->methodTable[methodIndex * 3 + 2] = ctx->opCounter;
		}
		return pos - 1;
	}

	// ------------------------------------------------------
	// Convert token stream to bytecode
	// ------------------------------------------------------
	int convert(ASMToken* tokens, int num, ASMParserContext* ctx) {
		ctx->numVars = 0;
		ctx->numOperations = 0;
		ctx->numTemp = 0;
		ctx->numMethods = 0;
		int cnt = 0;
		while (cnt < num) {
			ASMToken t = tokens[cnt++];
			switch (t.type) {
				case ASMToken::VARIABLE: cnt = parse_const(tokens, cnt, ctx);
					break;
				case ASMToken::STRUCT: cnt = parse_struct(tokens, cnt, ctx);
					break;
				case ASMToken::METHOD: cnt = parse_method(tokens, cnt, ctx);
					break;
				default: ++cnt;
			}
			if (ctx->errorCode != EC_OK) {
				LOGE << "Error detected - stopping now";
				return -1;
			}
		}
		return 0;
	}

	// -------------------------------------------------------
	// load script and convert it to bytecode
	// -------------------------------------------------------
	bool loadScript(const char* fileName, ASMScript* runtime) {
		std::string line;
		std::string result = "";
		std::ifstream myfile(fileName);
		if (myfile.is_open()) {
			while (std::getline(myfile, line)) {
				if (line.find("//") == std::string::npos) {
					result += line;
				}
			}
			myfile.close();
		}
		else {
			LOGE << "File " << fileName << " not found!!";
			return false;
		}
		ASMParserContext ctx;
		ASMToken token[512];
		int num = parse(result.c_str(), token, 512);
		if (convert(token, num, &ctx) == 0) {
			runtime->numOperations = ctx.numOperations;
			runtime->numVars = ctx.numVars;
			for (int i = 0; i < ctx.numVars; ++i) {
				runtime->varData[i] = ctx.varData[i];
				runtime->varHash[i] = ctx.varHash[i];
			}
			int total = ctx.numOperations * 14;
			for (int i = 0; i < total; ++i) {
				runtime->operations[i] = ctx.operations[i];
			}
			runtime->numMethods = ctx.numMethods;
			for (int i = 0; i < ctx.numMethods * 3; ++i) {
				runtime->methodTable[i] = ctx.methodTable[i];
			}
			return true;
		}
		return false;
	}

	// -------------------------------------------------------
	// debug script
	// -------------------------------------------------------
	void debug(const ASMScript& ctx) {
		LOG << "variables: " << ctx.numVars;
		for (int i = 0; i < ctx.numVars; ++i) {
			LOG << i << " = " << ctx.varHash[i] << " " << ctx.varData[i].x << " " << ctx.varData[i].y << " " << ctx.varData[i].z << " " << ctx.varData[i].w;
		}
		LOG << "methods: " << ctx.numMethods;
		for (int j = 0; j < ctx.numMethods; ++j) {
			LOG << "  method: " << ctx.methodTable[j * 3] << " start: " << ctx.methodTable[j * 3 + 1] << " end: " << ctx.methodTable[j * 3 + 2];
			LOG << "  operation dump:";
			for (int i = ctx.methodTable[j * 3 + 1]; i < ctx.methodTable[j * 3 + 2]; ++i) {
				LOG << " function: " << ctx.operations[i * 14 + 1]
					<< " result: " << ctx.operations[i * 14 + 2] << " " << ctx.operations[i * 14 + 3] << " " << ctx.operations[i * 14 + 4]
					<< " op1: " << ctx.operations[i * 14 + 5] << " " << ctx.operations[i * 14 + 6] << " " << ctx.operations[i * 14 + 7]
					<< " op2: " << ctx.operations[i * 14 + 8] << " " << ctx.operations[i * 14 + 9] << " " << ctx.operations[i * 14 + 10]
					<< " op3: " << ctx.operations[i * 14 + 11] << " " << ctx.operations[i * 14 + 12] << " " << ctx.operations[i * 14 + 13];
			}
		}

	}

	// -------------------------------------------------------
	// debug defined temp data channel
	// -------------------------------------------------------
	void debugTempData(const ASMScript& ctx, int index) {
		LOG << "Temp Data - index " << index;
		for (int j = 0; j < ASM_MAX_STEPS; ++j) {
			LOG << j << " : " << ctx.tempVarData[index][j].x << " " << ctx.tempVarData[index][j].y << " " << ctx.tempVarData[index][j].z;
		}
	}

	// -------------------------------------------------------
	// calculate pointer
	// -------------------------------------------------------
	Vector4f* calculate_pointer(int index, ASMScript& ctx, NParticleArray& array, int start) {
		// 0 = temp var
		// 1 = variable
		// 2 = IN var
		// 3 = OUT var
		// 4 = COUNTER
		// 5 = constant
		int idx = ctx.operations[index + 1];
		switch (ctx.operations[index]) {
			case 0: return &ctx.tempVarData[idx][0];
			case 1: return &ctx.varData[idx];
			case 2:
			case 3: return array.begin(idx) + start;
			case 4: return &ctx.counter;
			case 5: return &CONSTANTS[idx].value;
			default: return 0;
		}
	}

	// -------------------------------------------------------
	// execute operation
	// -------------------------------------------------------
	void execute(int step, ASMScript& ctx, NParticleArray& array, int start, int end) {
		int index = step * 14;
		int steps = end - start;
		Vector4f* r = calculate_pointer(index + 2, ctx, array, start);
		int rs = ctx.operations[index + 4];
		Vector4f* a = calculate_pointer(index + 5, ctx, array, start);
		int as = ctx.operations[index + 7];
		Vector4f* b = calculate_pointer(index + 8, ctx, array, start);
		int bs = ctx.operations[index + 10];
		Vector4f* c = calculate_pointer(index + 11, ctx, array, start);
		int cs = ctx.operations[index + 13];
		for (int i = 0; i < steps; ++i) {
			ctx.counter = Vector4f(start + i);
			switch (ctx.operations[index + 1]) {
			case 0:
				*r = *a + *b;
				break;
			case 1:
				*r = *a - *b;
				break;
			case 2:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->data[j] * b->data[j];
				}
				break;
			case 3:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->data[j] / b->data[j];
				}
				break;
			case 4:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = sin(a->data[j]);
				}
				break;
			case 5:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->x;
				}
				break;
			case 6:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->y;
				}
				break;
			case 7:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->z;
				}
				break;
			case 8:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = ds::math::random(a->data[j], b->data[j]);
				}
				break;
			case 9:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->data[j];
				}
				break;
			case 10:
				r->x = cos(a->x);
				r->y = sin(a->y);
				r->z = sin(a->z);
				r->w = 0.0f;
				break;
			case 11:
			{
					   Vector4f* time = array.begin(4) + start + i;
					   for (int j = 0; j < 4; ++j) {
						   r->data[j] = a->data[j] / 255.0f + (b->data[j] / 255.0f - a->data[j] / 255.0f) * time->z;
					   }
			}
				break;
			case 12:
				for (int j = 0; j < 4; ++j) {
					r->data[j] = a->data[j] / 255.0f + (b->data[j] / 255.0f - a->data[j] / 255.0f) * c->data[j];
				}
				break;
			case 13:
			{
				ds::Rect rct(a->x, a->y, a->z, a->w);
				*r = ds::math::getTextureCoordinates(rct);
			}
				break;
			}
			r += rs;
			a += as;
			b += bs;
			c += cs;
		}
	}

	// -------------------------------------------------------
	// find method in bytecode by name
	// -------------------------------------------------------
	int findMethod(const char* name, const ASMScript& script) {
		int hash = str::hash(name);
		int index = 0;
		for (int i = 0; i < script.numMethods; ++i) {
			if (script.methodTable[index] == hash) {
				return index;
			}
			index += 3;
		}
		return -1;
	}

	// -------------------------------------------------------
	// internal execute
	// -------------------------------------------------------
	bool execute(ASMScript& ctx, const char* methodName, NParticleArray& array, int start, int end) {
		int methodIndex = findMethod(methodName, ctx);
		if (methodIndex != -1) {
			ctx.clear();
			int operationStart = ctx.methodTable[methodIndex + 1];
			int operationEnd = ctx.methodTable[methodIndex + 2];
			int total = end - start;
			int steps = total / ASM_MAX_STEPS;
			int mo = total % ASM_MAX_STEPS;
			end = start + ASM_MAX_STEPS;
			if (end > array.maxSize()) {
				end = array.maxSize();
			}
			for (int i = 0; i < steps; ++i) {

				for (int i = operationStart; i < operationEnd; ++i) {
					execute(i, ctx, array, start, end);
				}
				start += ASM_MAX_STEPS;
				end += ASM_MAX_STEPS;
			}
			if (end > array.maxSize()) {
				end = array.maxSize();
			}
			if (mo > 0) {
				end = start + mo;
				for (int i = operationStart; i < operationEnd; ++i) {
					execute(i, ctx, array, start, end);
				}
			}
			return true;
		}
		else {
			LOGE << "No method '" << methodName << "' found";
			return false;
		}
	}

	// -------------------------------------------------------
	// special emitter method
	// -------------------------------------------------------
	void emitt(ASMScript& ctx, const char* methodName, float x, float y, NParticleArray& array) {
		int start = array.size();
		Vector4f& def = ctx.varData[0];
		int total = (int)def.z;
		def.x = x;
		def.y = y;
		int end = start + total;
		if (execute(ctx, methodName, array, start, end)) {
			for (int i = start; i < end; ++i) {
				array.wake(i);
			}
		}
		/*
		Vector4f* rot = array.begin(3);
		for (int i = 0; i < array.size(); ++i) {
			LOG << i << " " << RADTODEG(rot->x);
			++rot;
		}
		*/
	}

	// -------------------------------------------------------
	// execute method by name
	// -------------------------------------------------------
	void execute(ASMScript& ctx, const char* methodName, NParticleArray& array) {
		execute(ctx, methodName, array, 0, array.size());
	}

	// -------------------------------------------------------
	// patch constant by name using float
	// -------------------------------------------------------
	void patch_constant(ASMScript& ctx, const char* name, float value) {
		int hash = str::hash(name);
		patch_constant(ctx, hash, Vector4f(value, value, value, value));
	}

	// -------------------------------------------------------
	// patch constant by name
	// -------------------------------------------------------
	void patch_constant(ASMScript& ctx, const char* name, const Vector4f& value) {
		int hash = str::hash(name);
		patch_constant(ctx, hash, value);
	}

	// -------------------------------------------------------
	// patch constant by hash
	// -------------------------------------------------------
	void patch_constant(ASMScript& ctx, int hash, const Vector4f& value) {
		for (int i = 0; i < ctx.numVars; ++i) {
			if (ctx.varHash[i] == hash) {
				ctx.varData[i] = value;
			}
		}
	}

}