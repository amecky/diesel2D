#include "ParticlesEditState.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\base\GameStateMachine.h"

namespace ds {

	ParticlesEditState::ParticlesEditState(ParticleManager* particles) : GameState("ParticlesEditState"), _particles(particles) {
		_dialogState = 1;
		_offset = 0;
		_dialogPos = v2(10, 710);
		_generator_offset = 0;
		_generator_state = 1;
		_modifier_offset = 0;
		_modifier_state = 1;
		_selected_id = -1;
	}


	ParticlesEditState::~ParticlesEditState() {
	}

	// --------------------------------------------
	// activate
	// --------------------------------------------
	void ParticlesEditState::activate() {
		_particles->fillModel(&_model);
	}

	// --------------------------------------------
	// activate
	// --------------------------------------------
	void ParticlesEditState::deactivate() {
		
	}

	// --------------------------------------------
	// update
	// --------------------------------------------
	int ParticlesEditState::update(float dt) {
		_particles->update(dt);
		return 0;
	}

	// --------------------------------------------
	// click
	// --------------------------------------------
	int ParticlesEditState::onGUIButton(ds::DialogID dlgID, int button) {
		return button;
	}

	void ParticlesEditState::renderSelection() {
		if (gui::begin("Particlesystem", &_dialogState)) {
			gui::ComboBox(DIALOG_MANAGER_ID + 1, &_model, &_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 4, "Load")) {
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 5, "Save")) {
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 6, "Add")) {
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 7, "Start")) {
				if (_model.hasSelection()) {
					int id = _model.getSelectedValue();
					float cx = renderer::getScreenWidth() * 0.5f;
					float cy = renderer::getScreenHeight() * 0.5f;
					v2 position = v2(cx, cy);
					_particles->start(id, position);
				}
			}
			gui::endGroup();
		}
		gui::end();
	}

	void ParticlesEditState::reloadNames() {
		bool reload = false;
		int id = _model.getSelectedValue();
		if (_selected_id != id) {
			_selected_id = id;
			reload = true;
		}
		NewParticleSystem* system = _particles->getParticleSystem(id);
		if (reload) {
			_modifier_names.clear();
			const Modifiers& modifiers = system->getModifiers();
			for (size_t i = 0; i < modifiers.size(); ++i) {
				_modifier_names.push_back(modifiers[i]->getName());
			}
		}
		if (reload) {
			_generator_names.clear();
			ParticleEmitter& emitter = system->getEmitter();
			const Generators& generators = emitter.getGenerators();
			for (size_t i = 0; i < generators.size(); ++i) {
				_generator_names.push_back(generators[i]->getName());
			}
		}
	}

	void ParticlesEditState::renderModifierSelection() {
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		if (gui::begin("Modifiers", &_dialogState)) {
			gui::ComboBox(SPRITE_TEMPLATES_ID + 6, _modifier_names, &_modifier_state, &_modifier_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 7, "Add")) {

			}
			else if (gui::Button(DIALOG_MANAGER_ID + 8, "Remove")) {
				system->removeModifierByName(_modifier_names[_modifier_state].c_str());
				_modifier_names.erase(_modifier_names.begin() + _modifier_state);
			}
			gui::endGroup();
		}
		gui::end();
	}


	void ParticlesEditState::renderModifierSettings() {		
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		if (_modifier_names[_modifier_state] == "LinearSize") {
			if (gui::begin("LinearSize modifier", &_dialogState)) {
				LinearSizeModifier* modifier = static_cast<LinearSizeModifier*>(system->getModifier(PMT_LINEAR_SIZE));
				LinearSizeModifierData* data = modifier->getData();
				gui::InputVec2(SPRITE_TEMPLATES_ID + 10, "Min scale", &data->minScale);
				gui::InputVec2(SPRITE_TEMPLATES_ID + 11, "Max scale", &data->maxScale);
			}
			gui::end();
		}
	}

	void ParticlesEditState::renderGeneratorSelection() {
		if (gui::begin("Generators", &_dialogState)) {
			gui::ComboBox(SPRITE_TEMPLATES_ID + 6, _generator_names, &_generator_state, &_generator_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 7, "Add")) {
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 8, "Remove")) {
			}
			gui::endGroup();
		}
		gui::end();
	}

	void ParticlesEditState::renderGeneratorSettings() {
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		ParticleEmitter& emitter = system->getEmitter();
		if (_generator_names[_generator_state] == "Lifetime") {
			if (gui::begin("Lifetime", &_dialogState)) {
				LifetimeGenerator* generator = static_cast<LifetimeGenerator*>(emitter.getGenerator(PGT_LIFETIME));
				LifetimeGeneratorData* data = generator->getData();
				gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "TTL", &data->ttl);
				gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
			}
			gui::end();
		}
		else if (_generator_names[_generator_state] == "RadialVelocity") {
			if (gui::begin("RadialVelocity", &_dialogState)) {
				RadialVelocityGenerator* generator = static_cast<RadialVelocityGenerator*>(emitter.getGenerator(PGT_RADIAL_VELOCITY));
				RadialVelocityGeneratorData* data = generator->getData();
				gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Velocity", &data->velocity);
				gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
			}
			gui::end();
		}
		else if (_generator_names[_generator_state] == "Ring") {
			if (gui::begin("RingGenerator", &_dialogState)) {
				RingGenerator* generator = static_cast<RingGenerator*>(emitter.getGenerator(PGT_RING));
				RingGeneratorData* data = generator->getData();
				gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Radius", &data->radius);
				gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
				gui::InputFloat(SPRITE_TEMPLATES_ID + 12, "Step", &data->step);
				gui::InputFloat(SPRITE_TEMPLATES_ID + 13, "Angle Variance", &data->angleVariance);
			}
			gui::end();
		}
		else if (_generator_names[_generator_state] == "Random") {
			if (gui::begin("RandomGenerator", &_dialogState)) {
				ParticleRandomGenerator* generator = static_cast<ParticleRandomGenerator*>(emitter.getGenerator(PGT_RANDOM));
				ParticleRandomGeneratorData* data = generator->getData();
				gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Min", &data->minRandom);
				gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Max", &data->maxRandom);
			}
			gui::end();
		}

		// Size
		// HSVColor
		// Color
		// Point
		// Circle
	}

	void ParticlesEditState::renderEmitterSettings() {
		if (gui::begin("System", &_dialogState)) {
			int id = _model.getSelectedValue();
			NewParticleSystem* system = _particles->getParticleSystem(id);
			ParticleEmitter& emitter = system->getEmitter();
			ParticleEmitterData& data = emitter.getEmitterData();
			gui::InputInt(SPRITE_TEMPLATES_ID + 2, "count", &data.count);
			gui::InputInt(SPRITE_TEMPLATES_ID + 3, "ejectionPeriod", &data.ejectionPeriod);
			gui::InputInt(SPRITE_TEMPLATES_ID + 4, "ejectionVariance", &data.ejectionVariance);
			gui::InputInt(SPRITE_TEMPLATES_ID + 5, "ejectionCounter", &data.ejectionCounter);
		}
		gui::end();
	}
	// --------------------------------------------
	// render
	// --------------------------------------------
	void ParticlesEditState::render() {
		gui::start(DIALOG_MANAGER_ID, &_dialogPos);
		int gen_id = -1;
		renderSelection();
		if (_model.hasSelection()) {
			int id = _model.getSelectedValue();
			NewParticleSystem* system = _particles->getParticleSystem(id);
			reloadNames();
			if (gui::begin("System", &_dialogState)) {
				gui::beginGroup();
				if (gui::Button(DIALOG_MANAGER_ID + 8, "Emitter")) {
					_part_selection = 1;
				}
				else if (gui::Button(DIALOG_MANAGER_ID + 9, "Generator")) {
					_part_selection = 2;
				}
				else if (gui::Button(DIALOG_MANAGER_ID + 10, "Modifiers")) {
					_part_selection = 3;
				}
				gui::endGroup();
			}
			gui::end();

			if (_part_selection == 1) {
				renderEmitterSettings();
			}
			else if (_part_selection == 2) {						
				renderGeneratorSelection();
			}
			else if (_part_selection == 3) {				
				renderModifierSelection();
			}


			if (_generator_state != -1 && _part_selection == 2) {
				renderGeneratorSettings();
			}
			if (_modifier_state != -1 && _part_selection == 3) {
				renderModifierSettings();
			}
		}

		_particles->render();
	}
}