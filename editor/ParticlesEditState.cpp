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
			gui::ComboBox(&_model, &_offset);
			buttonGroup();
			gui::beginGroup();
			if (gui::Button("Start")) {
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
			//const Modifiers& modifiers = system->getModifiers();
			//for (size_t i = 0; i < modifiers.size(); ++i) {
				//_modifier_names.push_back(modifiers[i]->getName());
			//}
		}
		if (reload) {
			_generator_names.clear();
			//ParticleEmitter& emitter = system->getEmitter();
			//const Generators& generators = emitter.getGenerators();
			//for (size_t i = 0; i < generators.size(); ++i) {
				//_generator_names.push_back(generators[i]->getName());
			//}
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
		gui::Header("Modifiers");
		gui::ComboBox(_modifier_names, &_modifier_state, &_modifier_offset);
		gui::beginGroup();
		if (gui::Button("Add")) {
			_show_add_modifier = true;
		}
		else if (gui::Button("Remove")) {
			system->removeModifierByName(_modifier_names[_modifier_state].c_str());
			_modifier_names.erase(_modifier_names.begin() + _modifier_state);
		}
		gui::endGroup();
		if (_show_add_modifier) {
			gui::Header("Add modifier");
			gui::ComboBox(_particles->getFactory().getModifierNames(), &_add_modifier_state, &_add_modifier_offset);
			gui::beginGroup();
			if (gui::Button("Add")) {
				if (_add_modifier_state != -1) {
					//ParticleModifier* modifier = modifier::create_by_name(_available_modifiers[_add_modifier_state].c_str());
					//if (modifier != 0) {
						//system->addModifier(modifier);
						//_modifier_names.push_back(_available_modifiers[_add_modifier_state]);
					//}
					_show_add_modifier = false;
				}
			}
			else if (gui::Button("Cancel")) {
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
		/*
		if (_modifier_names[_modifier_state] == "LinearSize") {
			gui::Header("Linear size modifier");
			LinearSizeModifier* modifier = static_cast<LinearSizeModifier*>(system->getModifier(PMT_LINEAR_SIZE));
			LinearSizeModifierData* data = modifier->getData();
			gui::InputVec2("Min scale", &data->minScale);
			gui::InputVec2("Max scale", &data->maxScale);
		}
		else if (_modifier_names[_modifier_state] == "LinearAlpha") {
			gui::Header("Linear alpha modifier");
			LinearAlphaModifier* modifier = static_cast<LinearAlphaModifier*>(system->getModifier(PMT_LINEAR_ALPHA));
			LinearAlphaModifierData* data = modifier->getData();
			gui::InputFloat("Start", &data->startAlpha);
			gui::InputFloat("End", &data->endAlpha);
		}
		else if (_modifier_names[_modifier_state] == "LinearColor") {
			gui::Header("Linear color modifier");
			LinearColorModifier* modifier = static_cast<LinearColorModifier*>(system->getModifier(PMT_LINEAR_COLOR));
			LinearColorModifierData* data = modifier->getData();
			gui::InputColor("Start", &data->startColor);
			gui::InputColor("End", &data->endColor);
		}
		else if (_modifier_names[_modifier_state] == "DampingVelocity") {
			gui::Header("Damping velocity modifier");
			DampingVelocityModifier* modifier = static_cast<DampingVelocityModifier*>(system->getModifier(PMT_DAMPING_VELOCITY));
			DampingVelocityModifierData* data = modifier->getData();
			gui::InputFloat("Damping", &data->damping);
		}
		*/
	}

	// --------------------------------------------
	// render generator selection
	// --------------------------------------------
	void ParticlesEditState::renderGeneratorSelection() {
		gui::Header("Generators");
		gui::ComboBox(_generator_names, &_generator_state, &_generator_offset);
		gui::beginGroup();
		if (gui::Button("Add")) {
			_show_add_generator = true;
		}
		else if (gui::Button("Remove")) {
		}
		gui::endGroup();
		if (_show_add_generator) {
			int id = _model.getSelectedValue();
			NewParticleSystem* system = _particles->getParticleSystem(id);
			//ParticleEmitter& emitter = system->getEmitter();
			gui::Header("Add generator");
			gui::ComboBox(_available_generators, &_add_generator_state, &_add_generator_offset);
			gui::beginGroup();
			if (gui::Button("Add")) {
				if (_add_generator_state != -1) {
					/*
					ParticleGenerator* generator = generator::create_by_name(_available_generators[_add_generator_state].c_str());
					if (generator != 0) {
						emitter.add(generator);
						_modifier_names.push_back(_available_generators[_add_generator_state]);
					}
					*/
					_show_add_generator = false;
				}
			}
			else if (gui::Button("Cancel")) {
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
		//ParticleEmitter& emitter = system->getEmitter();
		/*
		if (_generator_names[_generator_state] == "Lifetime") {
			gui::Header("Lifetime");
			LifetimeGenerator* generator = static_cast<LifetimeGenerator*>(emitter.getGenerator(PGT_LIFETIME));
			LifetimeGeneratorData* data = generator->getData();
			gui::InputFloat("TTL", &data->ttl);
			gui::InputFloat("Variance", &data->variance);
		}
		else if (_generator_names[_generator_state] == "RadialVelocity") {
			gui::Header("Radial velocity");
			RadialVelocityGenerator* generator = static_cast<RadialVelocityGenerator*>(emitter.getGenerator(PGT_RADIAL_VELOCITY));
			RadialVelocityGeneratorData* data = generator->getData();
			gui::InputFloat("Velocity", &data->velocity);
			gui::InputFloat("Variance", &data->variance);
		}
		else if (_generator_names[_generator_state] == "Ring") {
			gui::Header("Ring generator");
			RingGenerator* generator = static_cast<RingGenerator*>(emitter.getGenerator(PGT_RING));
			RingGeneratorData* data = generator->getData();
			gui::InputFloat("Radius", &data->radius);
			gui::InputFloat("Variance", &data->variance);
			gui::InputFloat("Step", &data->step);
			gui::InputFloat("Angle Variance", &data->angleVariance);
		}
		else if (_generator_names[_generator_state] == "Random") {
			gui::Header("Random generator");
			ParticleRandomGenerator* generator = static_cast<ParticleRandomGenerator*>(emitter.getGenerator(PGT_RANDOM));
			ParticleRandomGeneratorData* data = generator->getData();
			gui::InputFloat("Min", &data->minRandom);
			gui::InputFloat("Max", &data->maxRandom);
		}
		else if (_generator_names[_generator_state] == "Color") {
			gui::Header("Color generator");
			ColorGenerator* generator = static_cast<ColorGenerator*>(emitter.getGenerator(PGT_COLOR));
			ColorGeneratorData* data = generator->getData();
			gui::InputColor("Color", &data->color);
		}
		else if (_generator_names[_generator_state] == "Size") {
			gui::Header("Size generator");
			SizeGenerator* generator = static_cast<SizeGenerator*>(emitter.getGenerator(PGT_SIZE));
			SizeGeneratorData* data = generator->getData();
			gui::InputVec2("Scale", &data->scale);
			gui::InputVec2("Variance", &data->variance);
		}
		*/
		// HSVColor
		// Point
		// Circle
	}

	// --------------------------------------------
	// render emitter settings
	// --------------------------------------------
	void ParticlesEditState::renderEmitterSettings() {
		gui::Header("System");
		int id = _model.getSelectedValue();
		NewParticleSystem* system = _particles->getParticleSystem(id);
		//ParticleEmitter& emitter = system->getEmitter();
		ParticleEmitterData& data = system->getEmitterData();
		NewParticleSystemData& system_data = system->getParticleData();
		char buffer[32];
		sprintf_s(buffer, 32, "ID: %d", system->getID());
		gui::Label(buffer);
		gui::InputInt("count", &data.count);
		gui::InputInt("ejectionPeriod", &data.ejectionPeriod);
		gui::InputInt("ejectionVariance", &data.ejectionVariance);
		gui::InputInt("ejectionCounter", &data.ejectionCounter);
		ds::Rect r = system_data.texture.rect;
		gui::InputRect("TextureRect", &r);
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
			gui::Header("System");
			gui::beginGroup();
			if (gui::Button("Emitter")) {
				_part_selection = 1;
			}
			else if (gui::Button("Generator")) {
				_part_selection = 2;
			}
			else if (gui::Button("Modifiers")) {
				_part_selection = 3;
			}
			gui::endGroup();
			gui::beginGroup();
			if (gui::Button("Load")) {
				system->load();
				_selected_id = -1;
				reloadNames();
			}
			else if (gui::Button("Save")) {
				system->save();
			}
			else if (gui::Button("Import")) {
				system->importJSON();
				_selected_id = -1;
				reloadNames();
			}
			else if (gui::Button("Export")) {
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