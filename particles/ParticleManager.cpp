#include "ParticleManager.h"
#include "..\utils\Profiler.h"
#include "..\renderer\shader.h"
#include "..\math\matrix.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"
#include "..\utils\Log.h"

namespace ds {

	const int MAX_PARTICLE_SYSTEMS = 128;

	// --------------------------------------------------------------------------
	// constructor
	// --------------------------------------------------------------------------
	ParticleManager::ParticleManager() {		
		_systems = new NewParticleSystem*[MAX_PARTICLE_SYSTEMS];
		for (int i = 0; i < 128; ++i) {
			_systems[i] = 0;
		}
	}

	// --------------------------------------------------------------------------
	// destructor
	// --------------------------------------------------------------------------
	ParticleManager::~ParticleManager(void) {
		for (int i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] != 0) {
				delete _systems[i];
			}
			_systems[i] = 0;
		}
		delete[] _systems;
	}

	NewParticleSystem* ParticleManager::create(int id, const char* name) {
		NewParticleSystem* system = new NewParticleSystem(id, name, &_factory);
		return system;
	}

	NewParticleSystem* ParticleManager::create(const char* name) {
		int idx = -1;
		for (int i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] == 0 && idx == -1) {
				idx = i;
			}
		}
		if (idx != -1) {
			NewParticleSystem* system = new NewParticleSystem(idx, name, &_factory);
			_systems[idx] = system;
			return system;
		}
		return 0;
	}

	// --------------------------------------------------------------------------
	// start specific particlesystem
	// --------------------------------------------------------------------------
	void ParticleManager::start(uint32 id,const Vector3f& pos) {	
		ZoneTracker z("ParticleManager::start");
		NewParticleSystem* system = _systems[id];
		assert(system != 0);
		system->start(pos);
	}

	int ParticleManager::findGroup(uint32 id) {
		for (int i = 0; i < _groups.size(); ++i) {
			if (_groups[i].id == id) {
				return i;
			}
		}
		return -1;
	}
	// --------------------------------------------------------------------------
	// start specific particlesystem
	// --------------------------------------------------------------------------
	void ParticleManager::startGroup(uint32 id, const Vector3f& pos) {
		ZoneTracker z("ParticleManager::start");
		int idx = findGroup(id);
		if (idx != -1) {
			const ParticleSystemGroup& group = _groups[idx];
			for (int i = 0; i < group.systems.size(); ++i) {
				NewParticleSystem* system = _systems[group.systems[i]];
				assert(system != 0);
				system->start(pos);
			}
		}
	}

	// --------------------------------------------------------------------------
	// stop specific particlesystem
	// --------------------------------------------------------------------------
	void ParticleManager::stop(uint32 id) {
		//assert(m_Index[id] != -1);
		//NewParticleSystem* system = m_Systems[m_Index[id]];
		//system->stop();
	}

	void ParticleManager::fillModel(gui::ComponentModel<int>* model) {
		model->clear();
		char buffer[32];
		for (int i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] != 0) {
				sprintf_s(buffer, 32, "%s (%d)", _systems[i]->getDebugName(), _systems[i]->getID());
				LOG << "----> '" << buffer << "'";
				model->add(buffer, _systems[i]->getID());
			}
		}
	}

	bool ParticleManager::saveData(JSONWriter& writer) {
		for (int i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] != 0) {
				writer.startCategory(_systems[i]->getDebugName());
				writer.write("id", i);
				writer.write("file", _systems[i]->getDebugName());
				writer.endCategory();
			}
		}
		if (!_groups.empty()) {
			writer.startCategory("groups");
			for (int i = 0; i < _groups.size(); ++i) {
				const ParticleSystemGroup& group = _groups[i];
				writer.startCategory(group.name);
				writer.write("id", group.id);
				writer.write("systems", group.systems.data(), group.systems.size());
				writer.endCategory();
			}
			writer.endCategory();
		}
		return true;
	}

	bool ParticleManager::loadData(const JSONReader& reader) {
		LOG << "importing data";
		int cats[256];
		int num = reader.get_categories(cats, 256);
		for (int i = 0; i < num; ++i) {
			LOG << "name: " << reader.get_category_name(cats[i]);
			if (reader.matches(cats[i], "groups")) {
				int groups[256];
				int num_groups = reader.get_categories(groups, 256, cats[i]);
				for (int z = 0; z < num_groups; ++z) {
					const char* name = reader.get_category_name(groups[z]);
					ParticleSystemGroup group;
					sprintf_s(group.name, 20, "%s", name);
					int id = -1;
					reader.get_uint(groups[z], "id", &group.id);
					int sar[32];
					int nr_sar = reader.get_array(groups[z], "systems", sar, 32);
					for (int j = 0; j < nr_sar; ++j) {
						group.systems.push_back(sar[j]);
					}
					_groups.push_back(group);
				}
			}
			else {
				const char* name = reader.get_string(cats[i], "file");
				int id = -1;
				reader.get_int(cats[i], "id", &id);
				if (id != -1) {
					NewParticleSystem* system = create(id, name);
					LOG << "id: " << id << " name: " << name;
					system->load();
					_systems[id] = system;
				}
			}
		}
		return true;
	}

	void  ParticleManager::removeSystem(int id) {
		assert(_systems[id] != 0);
		NewParticleSystem* system = _systems[id];
		delete system;
		_systems[id] = 0;
	}
	
	// --------------------------------------------------------------------------
	// update
	// --------------------------------------------------------------------------
	void ParticleManager::update(float elapsed) {
		ZoneTracker z("ParticleManager::update");
		for (int i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] != 0) {
				if (_systems[i]->getCountAlive() > 0) {
					_systems[i]->update(elapsed);
				}
			}
		}
	}

	// --------------------------------------------------------------------------
	// render
	// --------------------------------------------------------------------------
	void ParticleManager::render() {
		sprites::flush();
		int batchSize = 0;
		begin();
		ZoneTracker z("ParticleManager::render");
		for (int i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] != 0) {
				const ParticleArray& array = _systems[i]->getArray();
				const Texture& t = _systems[i]->getTexture();
				if (array.countAlive > 0) {
					for (int j = 0; j < array.countAlive; ++j) {
						if (m_ParticleIndex > MAX_PARTICLES) {
							flush();
						}
						for (int k = 0; k < 4; ++k) {
							particles[m_ParticleIndex].x = array.position[j].x;
							particles[m_ParticleIndex].y = array.position[j].y;
							particles[m_ParticleIndex].z = array.position[j].z;
							particles[m_ParticleIndex].uv = t.getUV(k);
							particles[m_ParticleIndex].scale = array.scale[j];
							particles[m_ParticleIndex].dimension = t.dim;
							particles[m_ParticleIndex].rotationIndex.x = array.rotation[j];
							particles[m_ParticleIndex].rotationIndex.y = k;
							particles[m_ParticleIndex].color = array.color[j];
							++m_ParticleIndex;
						}
					}
				}
			}
		}
		if (m_ParticleIndex > 0) {
			flush();
		}
		renderer::setCurrentShader(renderer::getDefaultShaderID());
	}

	// --------------------------------------------------------------------------
	// debug
	// --------------------------------------------------------------------------
	void ParticleManager::debug() {
		LOG << "---- Particlesystems -----";
		for (size_t i = 0; i < MAX_PARTICLE_SYSTEMS; ++i) {
			if (_systems[i] != 0) {
				LOG << i << " = " << _systems[i]->getDebugName() << " - alive: " << _systems[i]->getCountAlive();
			}
		}
	}

	// --------------------------------------------------------------------------
	// begin rendering
	// --------------------------------------------------------------------------
	void ParticleManager::begin() {	
		m_ParticleIndex = 0;
	}

	// --------------------------------------------------------------------------
	// end rendering
	// --------------------------------------------------------------------------
	void ParticleManager::end() {
		if (m_ParticleIndex > 0) {
			ZoneTracker z("ParticleManager:end");
			renderer::setWorldMatrix(matrix::m4identity());
			renderer::fillBuffer(bufferIndex, particles, m_ParticleIndex);
			renderer::draw(descriptorIndex, bufferIndex,m_ParticleIndex, indexBufferIndex);
			renderer::drawCounter().particles += m_ParticleIndex;
		}
	}

	// --------------------------------------------------------------------------
	// flush
	// --------------------------------------------------------------------------
	void ParticleManager::flush() {
		end();
		begin();
	}

	void ParticleManager::init(const Descriptor& desc) {
		bufferIndex = renderer::createVertexBuffer(VD_PARTICLE, MAX_PARTICLES * 4, true);
		indexBufferIndex = renderer::getQuadIndexBufferIndex();
		descriptorIndex = renderer::addDescriptor(desc);
	}

	
}
