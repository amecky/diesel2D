#include "ParticleSystemEditor.h"

namespace ds {

	ParticleSystemEditor::ParticleSystemEditor(ParticleManager* particles) : _particles(particles) , _system(0) {
		reset();
		_all_generator_names = _particles->getFactory().getGeneratorNames();
		_all_modifier_names = _particles->getFactory().getModifierNames();
	}


	ParticleSystemEditor::~ParticleSystemEditor() {
	}

	// --------------------------------------------
	// set system
	// --------------------------------------------
	void ParticleSystemEditor::setSystem(int id) {
		_system = _particles->getParticleSystem(id);
		if (_system != 0) {
			_system->getGeneratorNames(_generator_names);
			_system->getModifierNames(_modifier_names);
		}
		reset();
	}

	// --------------------------------------------
	// reset
	// --------------------------------------------
	void ParticleSystemEditor::reset() {
		_part_selection = 0;
		_generator_offset = 0;
		_generator_selection = -1;
		_show_add_generator = false;
		_add_generator_offset = 0;
		_add_generator_selection = -1;
		_modifier_offset = 0;
		_modifier_selection = -1;
		_add_modifier_offset = 0;
		_add_modifier_state = 1;
		_show_add_modifier = false;
	}

	// --------------------------------------------
	// render generator selection
	// --------------------------------------------
	void ParticleSystemEditor::renderGeneratorSelection() {
		gui::Header("Generators");
		gui::ComboBox(_generator_names, &_generator_selection, &_generator_offset);
		gui::beginGroup();
		if (gui::Button("Add")) {
			_show_add_generator = true;
		}
		else if (gui::Button("Remove")) {
		}
		gui::endGroup();
		if (_show_add_generator) {
			gui::Header("Add generator");
			gui::ComboBox(_all_generator_names, &_add_generator_selection, &_add_generator_offset);
			gui::beginGroup();
			if (gui::Button("Add")) {
				if (_add_generator_selection != -1) {
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
	void ParticleSystemEditor::renderGeneratorSettings() {
		if (_generator_selection != -1 && _system != 0) {
			const GeneratorInstance& instance = _system->getGeneratorInstance(_generator_selection);
			if (instance.generator->getType() == PGT_LIFETIME) {
				LifetimeGeneratorData* data = static_cast<LifetimeGeneratorData*>(instance.data);
				gui::Header("Lifetime");
				gui::InputFloat("TTL", &data->ttl);
				gui::InputFloat("Variance", &data->variance);
			}
			else if (instance.generator->getType() == PGT_RADIAL_VELOCITY) {
				RadialVelocityGeneratorData* data = static_cast<RadialVelocityGeneratorData*>(instance.data);
				gui::Header("Radial velocity");
				gui::InputFloat("Velocity", &data->velocity);
				gui::InputFloat("Variance", &data->variance);
			}
			else if (instance.generator->getType() == PGT_RING) {
				RingGeneratorData* data = static_cast<RingGeneratorData*>(instance.data);
				gui::Header("Ring generator");
				gui::InputFloat("Radius", &data->radius);
				gui::InputFloat("Variance", &data->variance);
				gui::InputFloat("Step", &data->step);
				gui::InputFloat("Angle Variance", &data->angleVariance);
			}
			else if (instance.generator->getType() == PGT_RANDOM) {
				gui::Header("Random generator");
				ParticleRandomGeneratorData* data = static_cast<ParticleRandomGeneratorData*>(instance.data);
				gui::InputFloat("Min", &data->minRandom);
				gui::InputFloat("Max", &data->maxRandom);
			}
			else if (instance.generator->getType() == PGT_COLOR) {
				gui::Header("Color generator");
				ColorGeneratorData* data = static_cast<ColorGeneratorData*>(instance.data);
				gui::InputColor("Color", &data->color);
			}
			else if (instance.generator->getType() == PGT_HSV_COLOR) {
				gui::Header("HSV Color generator");
				HSVColorGeneratorData* data = static_cast<HSVColorGeneratorData*>(instance.data);
				gui::InputVec3("HSV", &data->hsv);
				gui::InputFloat("hue variance", &data->hueVariance);
				gui::InputFloat("saturationVariance", &data->saturationVariance);
				gui::InputFloat("valueVariance", &data->valueVariance);
				gui::InputFloat("alpha", &data->alpha);

			}
			else if (instance.generator->getType() == PGT_SIZE) {
				gui::Header("Size generator");
				SizeGeneratorData* data = static_cast<SizeGeneratorData*>(instance.data);
				gui::InputVec2("Scale", &data->scale);
				gui::InputVec2("Variance", &data->variance);
			}
		}
		// Point
		// Circle
	}

	// --------------------------------------------
	// render modifier selection
	// --------------------------------------------
	void ParticleSystemEditor::renderModifierSelection() {
		gui::Header("Modifiers");
		gui::ComboBox(_modifier_names, &_modifier_selection, &_modifier_offset);
		gui::beginGroup();
		if (gui::Button("Add")) {
			_show_add_modifier = true;
		}
		else if (gui::Button("Remove")) {
			_system->removeModifierByName(_modifier_names[_modifier_selection].c_str());
			_modifier_names.erase(_modifier_names.begin() + _modifier_selection);
		}
		gui::endGroup();
		if (_show_add_modifier) {
			gui::Header("Add modifier");
			gui::ComboBox(_all_modifier_names, &_add_modifier_state, &_add_modifier_offset);
			gui::beginGroup();
			if (gui::Button("Add")) {
				if (_add_modifier_state != -1) {
					if (_particles->getFactory().addModifier(_system, _all_modifier_names[_add_modifier_state].c_str())) {
						_modifier_names.push_back(_all_modifier_names[_add_modifier_state]);
					}
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
	void ParticleSystemEditor::renderModifierSettings() {
		if (_modifier_selection != -1 && _system != 0) {
			const ModifierInstance& instance = _system->getModifierInstance(_modifier_selection);
			if (instance.modifier != 0) {
				if (instance.modifier->getType() == PMT_LINEAR_SIZE) {
					gui::Header("Linear size modifier");
					LinearSizeModifierData* data = static_cast<LinearSizeModifierData*>(instance.data);
					gui::InputVec2("Min scale", &data->minScale);
					gui::InputVec2("Max scale", &data->maxScale);
				}
				else if (instance.modifier->getType() == PMT_LINEAR_COLOR) {
					gui::Header("Linear color modifier");
					LinearColorModifierData* data = static_cast<LinearColorModifierData*>(instance.data);
					gui::InputColor("Start", &data->startColor);
					gui::InputColor("End", &data->endColor);
				}
				else if (instance.modifier->getType() == PMT_DAMPING_VELOCITY) {
					DampingVelocityModifierData* data = static_cast<DampingVelocityModifierData*>(instance.data);
					gui::Header("Damping velocity modifier");
					gui::InputFloat("Damping", &data->damping);
				}
				else if (instance.modifier->getType() == PMT_LINEAR_ALPHA) {
					LinearAlphaModifierData* data = static_cast<LinearAlphaModifierData*>(instance.data);
					gui::Header("Linear alpha modifier");
					gui::InputFloat("Start", &data->startAlpha);
					gui::InputFloat("End", &data->endAlpha);
				}
			}
		}
	}

	// --------------------------------------------
	// render emitter settings
	// --------------------------------------------
	void ParticleSystemEditor::renderEmitterSettings() {
		if (_system != 0) {
			gui::Header("System");
			ParticleEmitterData& data = _system->getEmitterData();
			ParticleSystemData& system_data = _system->getParticleData();
			char buffer[32];
			sprintf_s(buffer, 32, "ID: %d", _system->getID());
			gui::Label(buffer);
			gui::InputInt("count", &data.count);
			gui::InputInt("ejectionPeriod", &data.ejectionPeriod);
			gui::InputInt("ejectionVariance", &data.ejectionVariance);
			gui::InputInt("ejectionCounter", &data.ejectionCounter);
			ds::Rect r = system_data.texture.rect;
			gui::InputRect("TextureRect", &r);
			system_data.texture = math::buildTexture(r);
		}
	}

	void ParticleSystemEditor::render() {
		if (_system != 0) {
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
				_system->load();
				reset();
			}
			else if (gui::Button("Save")) {
				_system->save();
			}
			else if (gui::Button("Import")) {
				_system->importJSON();
				reset();
			}
			else if (gui::Button("Export")) {
				_system->exportJSON();
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
			
			if (_generator_selection != -1 && _part_selection == 2) {
				renderGeneratorSettings();
			}
			if (_modifier_selection != -1 && _part_selection == 3) {
				renderModifierSettings();
			}
		}
	}

}