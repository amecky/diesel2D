#pragma once
#include "..\base\GameState.h"
#include "..\particles\ParticleManager.h"
#include "..\ui\IMGUI.h"
#include "AssetEditor.h"

namespace ds {

	class ParticlesEditState : public GameState , public AssetEditor {

	public:
		ParticlesEditState(ParticleManager* particles);
		~ParticlesEditState();
		int update(float dt);
		void render();
		int onGUIButton(DialogID dlgID, int button);
		void activate();
		void deactivate();

		void showDialog() {}

		void init();
	private:
		void renderSelection();
		void reloadNames();
		void renderModifierSelection();
		void renderGeneratorSelection();
		void renderModifierSettings();
		void renderGeneratorSettings();
		void renderEmitterSettings();

		ParticleManager* _particles;
		gui::ComponentModel<int> _model;
		gui::InputDialog _dialog;
		v2 _dialogPos;
		int _dialogState;
		int _offset;
		int _generator_offset;
		int _generator_state;
		int _modifier_offset;
		int _modifier_state;
		bool _show_add_modifier;
		int _add_modifier_state;
		int _add_modifier_offset;
		bool _show_add_generator;
		int _add_generator_state;
		int _add_generator_offset;
		int _selected_id;
		int _part_selection;
		std::vector<std::string> _generator_names;
		std::vector<std::string> _modifier_names;
		std::vector<std::string> _available_modifiers;
		std::vector<std::string> _available_generators;
	};

}

