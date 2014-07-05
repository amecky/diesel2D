#include "ParticleManager.h"
#include "..\utils\Profiler.h"

namespace ds {

ParticleManager::ParticleManager() {	
	for ( int i = 0; i < 128; ++i ) {
		m_Indices[i].id = i;
		m_Indices[i].index = i;
	}
	m_Counter = 0;
}


ParticleManager::~ParticleManager(void) {
	ParticleSystems::iterator it = m_Systems.begin();
	while ( it != m_Systems.end() ) {
		delete (*it);
		it = m_Systems.erase(it);
	}
}

void ParticleManager::start(uint32 id,const Vector2f& pos) {
	Index& in = m_Indices[id];
	ParticleSystem* system = m_Systems[in.index];
	system->start(pos);
}

void ParticleManager::load(BinaryLoader* loader) {
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == CHNK_PARTICLESYSTEM ) {
			int id = 0;
			loader->read(&id);
			std::string file;
			loader->read(file);
			int max = 512;
			loader->read(&max);
			Index& idx = m_Indices[id];
			idx.id = id;			
			idx.index = m_Counter;
			sprintf(idx.debugName,"%s",file.c_str());
			ParticleSystem* system = new ParticleSystem;
			system->setRenderer(m_Renderer);
			system->setMaxParticles(max);
			system->setBlendState(m_BlendState);
			system->setTextureID(m_TextureID);
			system->init();
			m_AssetCompiler->load(file.c_str(),system,CVT_PARTICLESYSTEM);
			m_Systems.push_back(system);
			++m_Counter;
		}
		loader->closeChunk();
	}		
}

void ParticleManager::update(float elapsed) {
	PR_START("ParticleManager::update")
	for ( int i = 0; i < m_Counter; ++i ) {
		m_Systems[i]->update(elapsed);
	}
	PR_END("ParticleManager::update")
}

void ParticleManager::render() {
	PR_START("ParticleManager::render")
	for ( int i = 0; i < m_Counter; ++i ) {
		m_Systems[i]->render();
	}
	PR_END("ParticleManager::render")
}

void ParticleManager::debug() {
	LOG << "---- Particlesystems -----";
	LOG << "Number of systems: " << m_Counter;
	for ( int i = 0; i < m_Counter; ++i ) {
		Index& in = m_Indices[i];
		LOG << in.id << " : " << in.debugName;
	}
}

}
