#pragma once
#include "..\particles\ParticleManager.h"
#include "..\lib\collection_types.h"

namespace ds {

	class ParticleSystemEditor {

	public:
		ParticleSystemEditor(ParticleManager* particles);
		~ParticleSystemEditor();
		void setSystem(int id);
		void render();
	private:
		void renderEmitterSettings();
		void renderGeneratorSelection();
		void renderGeneratorSettings();
		void renderModifierSelection();
		void renderModifierSettings();
		void reset();

		ParticleManager* _particles;
		NewParticleSystem* _system;
		int _part_selection;
		int _generator_offset;
		int _generator_selection;
		bool _show_add_generator;
		int _add_generator_selection;
		int _add_generator_offset;
		Array<const char*> _generator_names;
		int _modifier_offset;
		int _modifier_selection;
		bool _show_add_modifier;
		int _add_modifier_state;
		int _add_modifier_offset;
		Array<const char*> _modifier_names;

		//Array<std::string> _all_generator_names;
		//Array<std::string> _all_modifier_names;
	};

}