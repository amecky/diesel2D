#include "ParticlesEditState.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\base\GameStateMachine.h"

namespace ds {

	ParticlesEditState::ParticlesEditState(ParticleManager* particles) : AssetEditor("ParticlesEditor", particles), _particles(particles) {
		_editor = new ParticleSystemEditor(particles);
		_dialogState = 1;
		_offset = 0;
		_dialogPos = v2(10, 710);		
		_current_system = 0;
	}


	ParticlesEditState::~ParticlesEditState() {
		delete _editor;
	}

	// --------------------------------------------
	// render selection
	// --------------------------------------------
	void ParticlesEditState::renderSelection() {
		if (gui::begin("Particlesystem", &_dialogState)) {
			gui::ComboBox(&_model, &_offset);
			buttonGroup();
			gui::beginGroup();
			if (gui::Button("Start")) {
				if (_current_system != 0 ) {
					float cx = renderer::getScreenWidth() * 0.5f;
					float cy = renderer::getScreenHeight() * 0.5f;
					v2 position = v2(cx, cy);
					_particles->start(_current_system->getID(), position);
				}
			}
			if (gui::Button("Remove")) {
				if (_model.hasSelection()) {
					int id = _model.getSelectedValue();
					_particles->removeSystem(id);
					_model.remove(id);
				}
			}
			gui::endGroup();
		}
		if (_showAdd) {
			int ret = _dialog.showEmbedded("Please provide a name", "Name");
			if (ret == 1) {
				NewParticleSystem* system = _particles->create(_dialog.getText());
				if (system != 0) {
					char buffer[64];
					sprintf_s(buffer, 64, "%s (%d)", system->getDebugName(), system->getID());
					_model.add(buffer, system->getID());
					_editor->setSystem(system->getID());
				}
				_showAdd = false;
			}
			if (ret == 2) {
				_showAdd = false;
			}
		}
	}

	// --------------------------------------------
	// reload names
	// --------------------------------------------
	void ParticlesEditState::reloadNames() {
		int id = _model.getSelectedValue();
		if (_selected_id != id) {
			_selected_id = id;
			_current_system = _particles->getParticleSystem(id);
			_editor->setSystem(id);
		}
	}

	void ParticlesEditState::init() {
		_particles->fillModel(&_model);
	}
	
	// --------------------------------------------
	// render
	// --------------------------------------------
	void ParticlesEditState::showDialog() {
		gui::start(209, &_dialogPos);
		int gen_id = -1;
		renderSelection();
		if (_model.hasSelection()) {
			reloadNames();
		}
		_editor->render();
		gui::end();
		_particles->render();
	}
}