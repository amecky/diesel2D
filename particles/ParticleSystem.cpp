#include "ParticleSystem.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\utils\Profiler.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include <stdarg.h>

namespace ds {

	const float VP_ARRAY[] = {
		-0.5f, 0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f, -0.5f
	};

	NewParticleSystem::NewParticleSystem(int id, const char* name, ParticleSystemFactory* factory) {
		strcpy_s(m_DebugName, 32, name);
		sprintf_s(_json_name, 64, "particles\\%s.json", name);
		_id = id;
		m_Array.initialize(MAX_PARTICLES);
		_count_modifiers = 0;
		_count_generators = 0;
		_factory = factory;

	}

	NewParticleSystem::~NewParticleSystem() {
		clear();
	}
	// -------------------------------------------------
	// start new instance
	// -------------------------------------------------
	ID NewParticleSystem::start(const Vector3f& startPosition) {
		if (_emitter_instances.numObjects < 64) {
			ID id = _emitter_instances.add();
			ParticleEmitterInstance& instance = _emitter_instances.get(id);
			instance.timer = 0.0f;
			instance.accumulated = 0.0f;
			instance.pos = startPosition;
			PR_START("NPS:emitterGenerate");
			uint32 start = 0;
			uint32 end = 0;
			emittParticles(instance, 0.0f, &start, &end);
			PR_END("NPS:emitterGenerate");
			PR_START("NPS:emitterModifiers");
			for (int i = 0; i < _count_modifiers; ++i) {
				const ModifierInstance& instance = _modifier_instances[i];
				instance.modifier->init(&m_Array, instance.data, start, end);
			}
			PR_END("NPS:emitterModifiers");
			return id;
		}
		else {
			return INVALID_ID;
		}
	}

	ParticleModifierData* NewParticleSystem::getData(const char* modifierName) {
		for (int i = 0; i < _count_modifiers; ++i) {
			const ModifierInstance& instance = _modifier_instances[i];
			if (strcmp(instance.modifier->getName(), modifierName) == 0) {
				return instance.data;
			}
		}
		return 0;
	}

	ParticleGeneratorData* NewParticleSystem::getGeneratorData(const char* generatorName) {
		for (int i = 0; i < _count_generators; ++i) {
			const GeneratorInstance& instance = _generator_instances[i];
			if (strcmp(instance.generator->getName(), generatorName) == 0) {
				return instance.data;
			}
		}
		return 0;
	}

	// -------------------------------------------------
	// tick emitter instances
	// -------------------------------------------------
	void NewParticleSystem::tickEmitters(float dt) {
		EmitterInstances::iterator it = _emitter_instances.begin();
		while (it != _emitter_instances.end()) {
			it->timer += dt;
			if (_emitter_data.duration >= 0.0f && it->timer > _emitter_data.duration) {
				it = _emitter_instances.remove(it->id);
			}
			else {
				++it;
			}
		}
	}

	// -------------------------------------------------------
	// generate
	// -------------------------------------------------------
	void NewParticleSystem::emittParticles(ParticleEmitterInstance& instance, float dt, uint32* start, uint32* end) {
		if (_emitter_data.frequency > 0.0f) {
			instance.accumulated += _emitter_data.frequency;
			if (instance.accumulated >= _emitter_data.frequency) {
				int count = (int)instance.accumulated;
				instance.accumulated -= count;
				emittParticles(instance, count, start, end, dt);
			}
		}
		else {
			emittParticles(instance, _emitter_data.count, start, end, dt);
		}
	}

	// -------------------------------------------------------
	// generate
	// -------------------------------------------------------
	void NewParticleSystem::emittParticles(const ParticleEmitterInstance& instance, int count, uint32* start, uint32* end, float dt) {
		*start = m_Array.countAlive;
		*end = *start + count;
		if (*end > m_Array.count) {
			*end = m_Array.count;
		}
		for (uint32 i = *start; i < *end; ++i) {
			m_Array.color[i] = Color::WHITE;
			m_Array.scale[i] = v2(1, 1);
			m_Array.baseScale[i] = v2(1, 1);
			m_Array.rotation[i] = 0.0f;
			m_Array.rotationVelocity[i] = 0.0f;
			m_Array.timer[i] = v3(0, 1, 1);
			m_Array.acceleration[i] = v3(0, 0, 0);
			m_Array.velocity[i] = v3(0, 0, 0);
			m_Array.position[i] = instance.pos;
		}
		for (uint32 i = *start; i < *end; ++i) {
			m_Array.wake(i);
		}
		for (int i = 0; i < _count_generators; ++i) {
			const GeneratorInstance& instance = _generator_instances[i];
			instance.generator->generate(&m_Array, instance.data, 0.0f, *start, *end);
		}
	}

	void NewParticleSystem::update(float elapsed) {
		PR_START("NPS:update");
		uint32 start = 0;
		uint32 end = 0;
		tickEmitters(elapsed);
		for (int i = 0; i < _emitter_instances.numObjects; ++i) {
			ParticleEmitterInstance& instance = _emitter_instances.objects[i];
			emittParticles(instance, elapsed, &start, &end);
		}
		for (int i = 0; i < _count_modifiers; ++i) {
			const ModifierInstance& instance = _modifier_instances[i];
			instance.modifier->update(&m_Array, instance.data, elapsed);
		}
		PR_END("NPS:update");
	}

	void NewParticleSystem::render() {
		PR_START("NPS:render");
		if (m_Array.countAlive > 0) {
			ds::sprites::draw(_system_data.texture, m_Array);
			renderer::drawCounter().particles += m_Array.countAlive;
		}
		PR_END("NPS:render");
	}

	void NewParticleSystem::clear() {
		LOG << "clearing particlesystem: " << m_DebugName;
		for (int i = 0; i < _count_modifiers; ++i) {
			const ModifierInstance& instance = _modifier_instances[i];
			if (instance.data != 0) {
				delete instance.data;
			}
		}
		for (int i = 0; i < _count_generators; ++i) {
			const GeneratorInstance& instance = _generator_instances[i];
			if (instance.data != 0) {
				delete instance.data;
			}
		}
		_count_generators = 0;
		_count_modifiers = 0;
		_emitter_instances.clear();
	}

	void NewParticleSystem::removeModifierByName(const char* name) {
		
	}

	ParticleModifier* NewParticleSystem::getModifier(ParticleModifierType type) {
		
		return 0;
	}

	bool NewParticleSystem::saveData(JSONWriter& writer) {
		writer.startCategory("particlesystem");
		writer.write("max", 1024);
		writer.write("texture_id", _system_data.textureID);
		writer.write("texture_rect", _system_data.texture.rect);
		writer.startCategory("emitter");
		writer.write("count", _emitter_data.count);
		writer.write("ejection_period", _emitter_data.ejectionPeriod);
		writer.write("ejection_variance", _emitter_data.ejectionVariance);
		writer.write("ejection_counter", _emitter_data.ejectionCounter);
		writer.endCategory();
		writer.startCategory("generators");
		for (int i = 0; i < _count_generators; ++i) {
			const GeneratorInstance& instance = _generator_instances[i];
			writer.startCategory(instance.generator->getName());
			if (instance.data != 0) {
				instance.data->save(writer);
			}
			writer.endCategory();
		}
		writer.endCategory();
		writer.startCategory("modifiers");
		for (int i = 0; i < _count_modifiers; ++i) {
			const ModifierInstance& instance = _modifier_instances[i];
			writer.startCategory(instance.modifier->getName());
			if (instance.data != 0) {
				instance.data->save(writer);
			}
			writer.endCategory();
		}
		writer.endCategory();
		writer.endCategory();
		return true;
	}

	bool NewParticleSystem::loadData(JSONReader& reader) {
		LOG << "importing data";
		clear();
		Category* root = reader.getCategory("particlesystem");
		if (root != 0) {
			_system_data.textureID = root->getUInt32("texture_id", 0);
			Rect r = root->getRect("texture_rect");
			_system_data.texture = math::buildTexture(r);
			// read modifiers
			Category* modifiers = root->getChild("modifiers");
			if (modifiers != 0) {
				std::vector<Category*> categories = modifiers->getChildren();
				for (size_t i = 0; i < categories.size(); ++i) {
					Category* c = categories[i];
					if (_factory->addModifier(this, c->getName().c_str())) {
						ParticleModifierData* data = getData(c->getName().c_str());
						if (data != 0) {
							data->read(c);
						}
					}
					else {
						LOGE << "cannot find modifier: " << c->getName();
					}
				}
			}
			Category* generators = root->getChild("generators");
			if (generators != 0) {
				std::vector<Category*> categories = generators->getChildren();
				for (size_t i = 0; i < categories.size(); ++i) {
					Category* c = categories[i];
					if (_factory->addGenerator(this, c->getName().c_str())) {
						ParticleGeneratorData* data = getGeneratorData(c->getName().c_str());
						if (data != 0) {
							data->read(c);
						}
					}
					else {
						LOGE << "cannot find generators: " << c->getName();
					}
				}
			}
			// emitter
			Category* emitter = root->getChild("emitter");
			if (emitter != 0) {
				_emitter_data.count = emitter->getUInt32("count", 10);
				_emitter_data.ejectionPeriod = emitter->getUInt32("ejection_period", 0);
				_emitter_data.ejectionVariance = emitter->getUInt32("ejection_variance", 0);
				_emitter_data.ejectionCounter = emitter->getUInt32("ejection_counter", 0);
				_emitter_data.duration = emitter->getFloat("duration", 0.0f);
				initEmitterData();
			}
		}
		return true;
	}

	void NewParticleSystem::prepareVertices() {
		for (int i = 0; i < MAX_PARTICLES; ++i) {
			m_Array.vertices[i * 4].uv.x = _system_data.texture.uv.x;
			m_Array.vertices[i * 4].uv.y = _system_data.texture.uv.y;
			m_Array.vertices[i * 4 + 1].uv.x = _system_data.texture.uv.z;
			m_Array.vertices[i * 4 + 1].uv.y = _system_data.texture.uv.y;
			m_Array.vertices[i * 4 + 2].uv.x = _system_data.texture.uv.z;
			m_Array.vertices[i * 4 + 2].uv.y = _system_data.texture.uv.w;
			m_Array.vertices[i * 4 + 3].uv.x = _system_data.texture.uv.x;
			m_Array.vertices[i * 4 + 3].uv.y = _system_data.texture.uv.w;
		}
	}

	void NewParticleSystem::buildVertices() {
		PR_START("NPS:buildVertices");
		Vector3f p(0, 0 , 0);
		Vector3f dp(0, 0, 0);
		for (uint32 i = 0; i < m_Array.countAlive; ++i) {
			Vector3f cor = m_Array.position[i];
			//cor = cor - ds::renderer::getSelectedViewport().getPosition();
		
			for (int j = 0; j < 4; ++j) {
				p.x = VP_ARRAY[j * 2] * _system_data.texture.dim.x;
				p.y = VP_ARRAY[j * 2 + 1] * _system_data.texture.dim.y;
				//vector::srt(cor, p, m_Array.scale[i], m_Array.rotation[i], &dp);
				dp = vector::srt(cor, p, m_Array.scale[i].x, m_Array.scale[i].y, m_Array.rotation[i]);
				m_Array.vertices[i * 4 + j].x = dp.x;
				m_Array.vertices[i * 4 + j].y = dp.y;
				m_Array.vertices[i * 4 + j].z = dp.z;
				m_Array.vertices[i * 4 + j].color = m_Array.color[i];
			}
		}
		PR_END("NPS:buildVertices");
	}

	void NewParticleSystem::initEmitterData() {
		float total = static_cast<float>(_emitter_data.count);
		LOG << "--> total: " << total;
		LOG << "--> duration: " << _emitter_data.duration;
		if (_emitter_data.duration == 0.0f) {
			_emitter_data.burst = true;
			_emitter_data.endless = false;
			_emitter_data.frequency = 0.0f;
		}
		else if (_emitter_data.duration < 0.0f) {
			_emitter_data.endless = true;
			_emitter_data.burst = false;
			_emitter_data.frequency = total / 60.0f;
		}
		else {
			//m_Frequency = total / (m_EmitterData.duration * 60.0f);
			_emitter_data.frequency = total / 60.0f;
			_emitter_data.burst = false;
			_emitter_data.endless = false;
		}
		LOG << "--> burst: " << _emitter_data.burst;
		LOG << "--> endless: " << _emitter_data.endless;
		LOG << "--> frequency: " << _emitter_data.frequency;
	}

	void NewParticleSystem::getModifierNames(std::vector<std::string>& names) {
		names.clear();
		for (int i = 0; i < _count_modifiers; ++i) {
			names.push_back(_modifier_instances[i].modifier->getName());
		}
	}

	void NewParticleSystem::getGeneratorNames(std::vector<std::string>& names) {
		names.clear();
		for (int i = 0; i < _count_generators; ++i) {
			names.push_back(_generator_instances[i].generator->getName());
		}
	}

}
