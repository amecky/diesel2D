#pragma once
#include "..\base\GameState.h"
#include "..\particles\ParticleManager.h"
#include "..\ui\IMGUI.h"
#include "AssetEditor.h"
#include "ParticleSystemEditor.h"

namespace ds {

	class ParticlesEditState : public AssetEditor {

	public:
		ParticlesEditState(ParticleManager* particles);
		~ParticlesEditState();
		void showDialog();
		void init();
		const char* getShortcut() const {
			return "PS";
		}
	private:
		void renderSelection();
		void reloadNames();
		void renderModifierSelection();
		void renderGeneratorSelection();
		void renderModifierSettings();
		void renderGeneratorSettings();
		void renderEmitterSettings();

		ParticleSystemEditor* _editor;
		ParticleManager* _particles;
		NewParticleSystem* _current_system;
		gui::ComponentModel<int> _model;
		gui::InputDialog _dialog;
		v2 _dialogPos;
		int _dialogState;
		int _offset;
		int _selected_id;
	};

}

