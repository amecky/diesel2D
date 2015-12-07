#include "ParticleManager.h"
#include "..\utils\Profiler.h"
#include "..\renderer\shader.h"
#include "..\math\matrix.h"
#include "..\renderer\graphics.h"
#include "..\sprites\SpriteBatch.h"

namespace ds {

	// --------------------------------------------------------------------------
	// constructor
	// --------------------------------------------------------------------------
	ParticleManager::ParticleManager() {		
		for ( int i = 0; i < 512; ++i ) {
			m_Index[i] = -1;
		}
		_dialogState = 1;
		_offset = 0;
		_dialogPos = v2(512, 384);
	}

	// --------------------------------------------------------------------------
	// destructor
	// --------------------------------------------------------------------------
	ParticleManager::~ParticleManager(void) {
		ParticleSystems::iterator it = m_Systems.begin();
		while ( it != m_Systems.end() ) {
			delete (*it);
			it = m_Systems.erase(it);
		}
		//SAFE_RELEASE(vertexBuffer);
	}

	// --------------------------------------------------------------------------
	// start specific particlesystem
	// --------------------------------------------------------------------------
	void ParticleManager::start(uint32 id,const Vector3f& pos) {	
		PR_START("ParticleManager::start");
		assert(m_Index[id] != -1);
		NewParticleSystem* system = m_Systems[m_Index[id]];
		system->start(pos);
		PR_END("ParticleManager::start");
	}

	void ParticleManager::start(uint32 id, const ParticleGeneratorData& data) {
		PR_START("ParticleManager::start");
		assert(m_Index[id] != -1);
		NewParticleSystem* system = m_Systems[m_Index[id]];
		system->start(data);
		PR_END("ParticleManager::start");
	}

	// --------------------------------------------------------------------------
	// stop specific particlesystem
	// --------------------------------------------------------------------------
	void ParticleManager::stop(uint32 id) {
		assert(m_Index[id] != -1);
		NewParticleSystem* system = m_Systems[m_Index[id]];
		system->stop();
	}

	// --------------------------------------------------------------------------
	// load binary file
	// --------------------------------------------------------------------------
	void ParticleManager::load(BinaryLoader* loader) {
		char buffer[32];
		while ( loader->openChunk() == 0 ) {		
			if ( loader->getChunkID() == CHNK_PARTICLESYSTEM ) {
				int id = 0;
				loader->read(&id);
				std::string file;
				loader->read(file);
				int max = 512;
				loader->read(&max);
				LOG << "loading particle system: " << id << " file: " << file;
				// FIXME: check if we already have one with this id
				NewParticleSystem* system = new NewParticleSystem;
				system->setDebugName(file.c_str());
				ds::assets::load(file.c_str(),system,CVT_NPS);
				m_Index[id] = m_Systems.size();
				m_Systems.push_back(system);
				sprintf_s(buffer, 32, "%s (%d)", file.c_str(), id);
				LOG << "----> '" << buffer << "'";
				_model.add(buffer, id);
			}
			loader->closeChunk();
		}		
		LOG << "Number of particle systems: " << m_Systems.size();
	}

	// --------------------------------------------------------------------------
	// update
	// --------------------------------------------------------------------------
	void ParticleManager::update(float elapsed) {
		PR_START("ParticleManager::update");
		for ( size_t i = 0; i < m_Systems.size(); ++i ) {
			m_Systems[i]->update(elapsed);
		}
		PR_END("ParticleManager::update");
	}

	// --------------------------------------------------------------------------
	// render
	// --------------------------------------------------------------------------
	void ParticleManager::render() {
		sprites::flush();
		int batchSize = 0;
		begin();
		PR_START("ParticleManager::render");
		for (size_t i = 0; i < m_Systems.size(); ++i) {
			const ParticleArray& array = m_Systems[i]->getArray();
			const Texture& t = m_Systems[i]->getTexture();
			if (array.countAlive > 0) {
				for (int j = 0; j < array.countAlive; ++j) {
					if (m_ParticleIndex > 2048) {
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
		if (m_ParticleIndex > 0) {
			flush();
		}
		renderer::setCurrentShader(renderer::getDefaultShaderID());
		PR_END("ParticleManager::render");
	}

	// --------------------------------------------------------------------------
	// debug
	// --------------------------------------------------------------------------
	void ParticleManager::debug() {
		LOG << "---- Particlesystems -----";
		LOG << "Number of systems: " << m_Systems.size();
		for (int i = 0; i < 512; ++i) {
			if (m_Index[i] != -1) {
				LOG << i << " = " << m_Index[i];
			}
		}
		for (size_t i = 0; i < m_Systems.size(); ++i) {
			LOG << i << " = " << m_Systems[i]->getDebugName() << " - alive: " << m_Systems[i]->getCountAlive();
			// max / alive
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
			PR_START("ParticleManager");
			renderer::setWorldMatrix(matrix::m4identity());
			renderer::fillBuffer(bufferIndex, particles, m_ParticleIndex);
			renderer::draw(descriptorIndex, bufferIndex,m_ParticleIndex, indexBufferIndex);
			renderer::drawCounter().particles += m_ParticleIndex;
			PR_END("ParticleManager");
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
		bufferIndex = renderer::createVertexBuffer(VD_PARTICLE, 4096 * 4, true);
		indexBufferIndex = renderer::getQuadIndexBufferIndex();
		descriptorIndex = renderer::addDescriptor(desc);
	}

	void ParticleManager::showDialog() {
		gui::start(DIALOG_MANAGER_ID, &_dialogPos);
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
					start(id, v2(512, 384));
				}
			}
			gui::endGroup();
		}
		gui::end();
		if (_model.hasSelection()) {
			int id = _model.getSelectedValue();
			NewParticleSystem* system = m_Systems[id];
			if (gui::begin("System", &_dialogState)) {
				ParticleEmitter& emitter = system->getEmitter();
				ParticleEmitterData& data = emitter.getEmitterData();
				/*
				uint32 ejectionPeriod;
				uint32 ejectionVariance;
				uint32 ejectionCounter;
				uint32 count;
				float duration; // seconds
				int loops; // how many ticks to run
				*/
				gui::InputInt(SPRITE_TEMPLATES_ID + 2, "count", &data.count);
				gui::InputInt(SPRITE_TEMPLATES_ID + 3, "ejectionPeriod", &data.ejectionPeriod);
				gui::InputInt(SPRITE_TEMPLATES_ID + 4, "ejectionVariance", &data.ejectionVariance);
				gui::InputInt(SPRITE_TEMPLATES_ID + 5, "ejectionCounter", &data.ejectionCounter);
				const Generators& generators = emitter.getGenerators();
				std::vector<std::string> names;
				for (size_t i = 0; i < generators.size(); ++i) {
					names.push_back(generators[i]->getName());
				}
				int s = 0;
				int o = 0;
				gui::ComboBox(SPRITE_TEMPLATES_ID + 6, names, &s, &o);
				const Modifiers& modifiers = system->getModifiers();
				std::vector<std::string> mnames;
				for (size_t i = 0; i < modifiers.size(); ++i) {
					mnames.push_back(modifiers[i]->getName());
				}
				gui::ComboBox(SPRITE_TEMPLATES_ID + 7, mnames, &s, &o);
			}
			gui::end();
		}
	}
}
