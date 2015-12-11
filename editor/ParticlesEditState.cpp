#include "ParticlesEditState.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\base\GameStateMachine.h"

namespace ds {

	ParticlesEditState::ParticlesEditState(ParticleManager* particles) : GameState("ParticlesEditState"), AssetEditor(particles) , _particles(particles) {
		_dialogState = 1;
		_offset = 0;
		_dialogPos = v2(10, 710);
		_generator_offset = 0;
		_generator_state = 1;
		_modifier_offset = 0;
		_modifier_state = 1;
		_add_modifier_offset = 0;
		_add_modifier_state = 1;
		_add_generator_offset = 0;
		_add_generator_state = 1;
		_selected_id = -1;
		_available_modifiers.push_back("Position");
		_available_modifiers.push_back("LinearColor");
		_available_modifiers.push_back("LinearSize");
		_available_modifiers.push_back("PerpendicularMove");
		_available_modifiers.push_back("ColorPath");
		_available_modifiers.push_back("AlphaPath");
		_available_modifiers.push_back("DampingVelocity");
		_available_modifiers.push_back("SizePath");
		_available_modifiers.push_back("VelocityRotation");
		_available_modifiers.push_back("Time");
		_available_modifiers.push_back("LinearAlpha");
		_show_add_modifier = false;

		_available_generators.push_back("Ring");
		_available_generators.push_back("Circle");
		_available_generators.push_back("Line");
		_available_generators.push_back("Point");
		_available_generators.push_back("Sphere");
		_available_generators.push_back("RandomSphere");
		_available_generators.push_back("RadialVelocity");
		_available_generators.push_back("Velocity");
		_available_generators.push_back("Lifetime");
		_available_generators.push_back("Color");
		_available_generators.push_back("HSVColor");
		_available_generators.push_back("Size");
		_available_generators.push_back("Random");
		_show_add_generator = false;


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

	// --------------------------------------------
	// render selection
	// --------------------------------------------
	void ParticlesEditState::renderSelection() {
		if (gui::begin("Particlesystem", &_dialogState)) {
			gui::ComboBox(DIALOG_MANAGER_ID + 1, &_model, &_offset);
			buttonGroup(DIALOG_MANAGER_ID + 4);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 7, "Start")) {
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
		if (_showAdd) {
			int ret = _dialog.showEmbedded("Please provide a name", "Name");
			if (ret == 1) {
				//int id = _templates->createEmptyTemplate(_dialog.getText());
				//_model.add(_dialog.getText(), id);
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

	void ParticlesEditState::init() {
		_particles->fillModel(&_model);
	}
	// --------------------------------------------
	// render modifier selection
	// --------------------------------------------
	void ParticlesEditState::renderModifierSelection() {
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		gui::Header(SPRITE_TEMPLATES_ID + 14, "Modifiers");
		gui::ComboBox(SPRITE_TEMPLATES_ID + 6, _modifier_names, &_modifier_state, &_modifier_offset);
		gui::beginGroup();
		if (gui::Button(DIALOG_MANAGER_ID + 7, "Add")) {
			_show_add_modifier = true;
		}
		else if (gui::Button(DIALOG_MANAGER_ID + 8, "Remove")) {
			system->removeModifierByName(_modifier_names[_modifier_state].c_str());
			_modifier_names.erase(_modifier_names.begin() + _modifier_state);
		}
		gui::endGroup();
		if (_show_add_modifier) {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Add modifier");
			gui::ComboBox(SPRITE_TEMPLATES_ID + 9, _available_modifiers, &_add_modifier_state, &_add_modifier_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 10, "Add")) {
				if (_add_modifier_state != -1) {
					ParticleModifier* modifier = modifier::create_by_name(_available_modifiers[_add_modifier_state].c_str());
					if (modifier != 0) {
						system->addModifier(modifier);
						_modifier_names.push_back(_available_modifiers[_add_modifier_state]);
					}
					_show_add_modifier = false;
				}
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 11, "Cancel")) {
				_show_add_modifier = false;
			}
			gui::endGroup();
		}
	}

	// --------------------------------------------
	// render modifier settings
	// --------------------------------------------
	void ParticlesEditState::renderModifierSettings() {		
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		if (_modifier_names[_modifier_state] == "LinearSize") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Linear size modifier");
			LinearSizeModifier* modifier = static_cast<LinearSizeModifier*>(system->getModifier(PMT_LINEAR_SIZE));
			LinearSizeModifierData* data = modifier->getData();
			gui::InputVec2(SPRITE_TEMPLATES_ID + 10, "Min scale", &data->minScale);
			gui::InputVec2(SPRITE_TEMPLATES_ID + 11, "Max scale", &data->maxScale);
		}
		else if (_modifier_names[_modifier_state] == "LinearAlpha") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Linear alpha modifier");
			LinearAlphaModifier* modifier = static_cast<LinearAlphaModifier*>(system->getModifier(PMT_LINEAR_ALPHA));
			LinearAlphaModifierData* data = modifier->getData();
			gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Start", &data->startAlpha);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "End", &data->endAlpha);
		}
		else if (_modifier_names[_modifier_state] == "LinearColor") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Linear color modifier");
			LinearColorModifier* modifier = static_cast<LinearColorModifier*>(system->getModifier(PMT_LINEAR_COLOR));
			LinearColorModifierData* data = modifier->getData();
			gui::InputColor(SPRITE_TEMPLATES_ID + 10, "Start", &data->startColor);
			gui::InputColor(SPRITE_TEMPLATES_ID + 11, "End", &data->endColor);
		}
		else if (_modifier_names[_modifier_state] == "DampingVelocity") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Damping velocity modifier");
			DampingVelocityModifier* modifier = static_cast<DampingVelocityModifier*>(system->getModifier(PMT_DAMPING_VELOCITY));
			DampingVelocityModifierData* data = modifier->getData();
			gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Damping", &data->damping);
		}
	}

	// --------------------------------------------
	// render generator selection
	// --------------------------------------------
	void ParticlesEditState::renderGeneratorSelection() {
		gui::Header(SPRITE_TEMPLATES_ID + 14, "Generators");
		gui::ComboBox(SPRITE_TEMPLATES_ID + 6, _generator_names, &_generator_state, &_generator_offset);
		gui::beginGroup();
		if (gui::Button(DIALOG_MANAGER_ID + 7, "Add")) {
			_show_add_generator = true;
		}
		else if (gui::Button(DIALOG_MANAGER_ID + 8, "Remove")) {
		}
		gui::endGroup();
		if (_show_add_generator) {
			int id = _model.getSelectedValue();
			NewParticleSystem* system = _particles->getParticleSystem(id);
			ParticleEmitter& emitter = system->getEmitter();
			gui::Header(SPRITE_TEMPLATES_ID + 15, "Add generator");
			gui::ComboBox(SPRITE_TEMPLATES_ID + 9, _available_generators, &_add_generator_state, &_add_generator_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 10, "Add")) {
				if (_add_generator_state != -1) {
					ParticleGenerator* generator = generator::create_by_name(_available_generators[_add_generator_state].c_str());
					if (generator != 0) {
						emitter.add(generator);
						_modifier_names.push_back(_available_generators[_add_generator_state]);
					}
					_show_add_generator = false;
				}
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 11, "Cancel")) {
				_show_add_generator = false;
			}
			gui::endGroup();
		}
	}

	// --------------------------------------------
	// render generator settings
	// --------------------------------------------
	void ParticlesEditState::renderGeneratorSettings() {
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		ParticleEmitter& emitter = system->getEmitter();
		if (_generator_names[_generator_state] == "Lifetime") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Lifetime");
			LifetimeGenerator* generator = static_cast<LifetimeGenerator*>(emitter.getGenerator(PGT_LIFETIME));
			LifetimeGeneratorData* data = generator->getData();
			gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "TTL", &data->ttl);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
		}
		else if (_generator_names[_generator_state] == "RadialVelocity") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Radial velocity");
			RadialVelocityGenerator* generator = static_cast<RadialVelocityGenerator*>(emitter.getGenerator(PGT_RADIAL_VELOCITY));
			RadialVelocityGeneratorData* data = generator->getData();
			gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Velocity", &data->velocity);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
		}
		else if (_generator_names[_generator_state] == "Ring") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Ring generator");
			RingGenerator* generator = static_cast<RingGenerator*>(emitter.getGenerator(PGT_RING));
			RingGeneratorData* data = generator->getData();
			gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Radius", &data->radius);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 12, "Step", &data->step);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 13, "Angle Variance", &data->angleVariance);
		}
		else if (_generator_names[_generator_state] == "Random") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Random generator");
			ParticleRandomGenerator* generator = static_cast<ParticleRandomGenerator*>(emitter.getGenerator(PGT_RANDOM));
			ParticleRandomGeneratorData* data = generator->getData();
			gui::InputFloat(SPRITE_TEMPLATES_ID + 10, "Min", &data->minRandom);
			gui::InputFloat(SPRITE_TEMPLATES_ID + 11, "Max", &data->maxRandom);
		}
		else if (_generator_names[_generator_state] == "Color") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Color generator");
			ColorGenerator* generator = static_cast<ColorGenerator*>(emitter.getGenerator(PGT_COLOR));
			ColorGeneratorData* data = generator->getData();
			gui::InputColor(SPRITE_TEMPLATES_ID + 10, "Color", &data->color);
		}
		else if (_generator_names[_generator_state] == "Size") {
			gui::Header(SPRITE_TEMPLATES_ID + 14, "Size generator");
			SizeGenerator* generator = static_cast<SizeGenerator*>(emitter.getGenerator(PGT_SIZE));
			SizeGeneratorData* data = generator->getData();
			gui::InputVec2(SPRITE_TEMPLATES_ID + 10, "Scale", &data->scale);
			gui::InputVec2(SPRITE_TEMPLATES_ID + 11, "Variance", &data->variance);
		}
		// HSVColor
		// Point
		// Circle
	}

	// --------------------------------------------
	// render emitter settings
	// --------------------------------------------
	void ParticlesEditState::renderEmitterSettings() {
		gui::Header(SPRITE_TEMPLATES_ID + 6, "System");
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		ParticleEmitter& emitter = system->getEmitter();
		ParticleEmitterData& data = emitter.getEmitterData();
		NewParticleSystemData& system_data = system->getParticleData();
		char buffer[32];
		sprintf_s(buffer, 32, "ID: %d", system->getID());
		gui::Label(SPRITE_TEMPLATES_ID + 7, buffer);
		gui::InputInt(SPRITE_TEMPLATES_ID + 2, "count", &data.count);
		gui::InputInt(SPRITE_TEMPLATES_ID + 3, "ejectionPeriod", &data.ejectionPeriod);
		gui::InputInt(SPRITE_TEMPLATES_ID + 4, "ejectionVariance", &data.ejectionVariance);
		gui::InputInt(SPRITE_TEMPLATES_ID + 5, "ejectionCounter", &data.ejectionCounter);
		ds::Rect r = system_data.texture.rect;
		gui::InputRect(SPRITE_TEMPLATES_ID + 6, "TextureRect", &r);
		system_data.texture = math::buildTexture(r);
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
			gui::Header(DIALOG_MANAGER_ID + 7, "System");
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
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 11, "Load")) {
				system->load();
				_selected_id = -1;
				reloadNames();
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 12, "Save")) {
				system->save();
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 13, "Import")) {
				system->importJSON();
				_selected_id = -1;
				reloadNames();
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 14, "Export")) {
				system->exportJSON();
			}
			gui::endGroup();
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
		gui::end();
		_particles->render();
	}
}