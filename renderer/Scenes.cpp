#include "Scenes.h"
#include "..\io\FileRepository.h"
#include "..\io\json.h"
#include "..\utils\Log.h"

namespace ds {

	struct RenderCallDefinition {

		const char* name;
		int numVars;

		RenderCallDefinition(const char* n, int num) : name(n), numVars(num) {}

	};

	const RenderCallDefinition RCD_ARRAY[] = {
		{ "set_render_target", 0 },
		{ "world_render_layer", 0 },
		{ "set_render_target", 0 },
		{ "restore_backbuffer", 0 },
		{ "select_shader", 1 },
		{ "draw_screen_quad", 0 },
		{ "render_particles", 0 },
		{ "select_viewport", 1 }
	};

	const int NUMBER_RCD = 8;

	Scenes::Scenes() {
	}


	Scenes::~Scenes() {
	}

	void Scenes::load() {
		int size = -1;
		char* text = repository::load("content\\scenes.script", &size);
		if (size != -1) {
			Tokenizer tokenizer;
			tokenizer.parse(text);
			for (int i = 0; i < tokenizer.size(); ++i) {
				LOG << i << " = " << tokenizer.name(i);
			}
			delete[] text;
		}

	}

}