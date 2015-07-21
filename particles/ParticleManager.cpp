#include "ParticleManager.h"
#include "..\utils\Profiler.h"
#include "..\renderer\shader.h"
#include "..\math\matrix.h"

namespace ds {

	// --------------------------------------------------------------------------
	// constructor
	// --------------------------------------------------------------------------
	ParticleManager::ParticleManager() {		
		for ( int i = 0; i < 512; ++i ) {
			m_Index[i] = -1;
		}
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
		assert(m_Index[id] != -1);
		NewParticleSystem* system = m_Systems[m_Index[id]];
		system->start(pos);
	}

	void ParticleManager::start(uint32 id, const ParticleGeneratorData& data) {
		assert(m_Index[id] != -1);
		NewParticleSystem* system = m_Systems[m_Index[id]];
		system->start(data);
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
		while ( loader->openChunk() == 0 ) {		
			if ( loader->getChunkID() == CHNK_PARTICLESYSTEM ) {
				int id = 0;
				loader->read(&id);
				std::string file;
				loader->read(file);
				int max = 512;
				loader->read(&max);
				LOGC("ParticleManager") << "loading particle system: " << id << " file: " << file;
				// FIXME: check if we already have one with this id
				NewParticleSystem* system = new NewParticleSystem;
				system->setDebugName(file.c_str());
				ds::assets::load(file.c_str(),system,CVT_NPS);
				m_Index[id] = m_Systems.size();
				m_Systems.push_back(system);
			}
			loader->closeChunk();
		}		
		LOGC("ParticleManager") << "Number of particle systems: " << m_Systems.size();
	}

	// --------------------------------------------------------------------------
	// update
	// --------------------------------------------------------------------------
	void ParticleManager::update(float elapsed) {
		PR_START("ParticleManager::update")
		for ( size_t i = 0; i < m_Systems.size(); ++i ) {
			m_Systems[i]->update(elapsed);
		}
		PR_END("ParticleManager::update")
	}

	// --------------------------------------------------------------------------
	// render
	// --------------------------------------------------------------------------
	void ParticleManager::render() {
		sprites::flush();
		int batchSize = 0;
		begin();
		PR_START("ParticleManager::render")
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
		PR_END("ParticleManager::render")
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
			PR_START("ParticleManager")
			renderer::setWorldMatrix(matrix::m4identity());
			renderer::fillBuffer(bufferIndex, particles, m_ParticleIndex);
			renderer::draw(descriptorIndex, bufferIndex,m_ParticleIndex, indexBufferIndex);
			renderer::drawCounter().particles += m_ParticleIndex;
			PR_END("ParticleManager")
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
}
