#pragma once
#include "Tokenizer.h"

struct Script {

	Tokenizer tokenizer;
	std::vector<ScriptBlock> blocks;
	ScriptContext context;
	IdString hash;

	void run() {
		tokenizer.run(blocks,context,false);
	}
};

