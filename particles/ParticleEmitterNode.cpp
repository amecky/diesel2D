#include "ParticleEmitterNode.h"

namespace ds {
// -------------------------------------------------------
// 
// -------------------------------------------------------
ParticleEmitterNode::ParticleEmitterNode(Renderer* renderer,int material,uint32 maxQuads)
	: Node("ParticleEmitter",renderer,material) , m_IdCounter(0) {
	m_ParticleBuffer = new ParticleBuffer(renderer,maxQuads);
}

// -------------------------------------------------------
// Prepare rendering
// -------------------------------------------------------
void ParticleEmitterNode::prepareRendering() {
}

// -------------------------------------------------------
// Post rendering
// -------------------------------------------------------
void ParticleEmitterNode::postRendering() {
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void ParticleEmitterNode::draw() {
	m_ParticleBuffer->draw();
}

// -------------------------------------------------------
// Updates particle buffer and emitter
// -------------------------------------------------------
void ParticleEmitterNode::update(float elapsed) {
	m_ParticleBuffer->manageLifecyle(elapsed);	
	Particle* head = m_ParticleBuffer->getParticles();
	if ( head != 0 ) {
		for ( size_t i = 0; i < m_AffectorList.size(); ++i ) {
			ParticleAffector* pa = m_AffectorList[i];
			pa->update(head,elapsed);
		}
	}
	m_ParticleBuffer->moveParticles(elapsed);
	for ( size_t i = 0; i < m_EmitterList.size(); ++i ) {
		ParticleEmitter* emitter = m_EmitterList[i];
		emitter->update(m_ParticleBuffer,elapsed);
	}
}

// -------------------------------------------------------
// Emit particles - triggered from outside
// -------------------------------------------------------
void ParticleEmitterNode::emitParticles() {
	for ( size_t i = 0 ; i < m_EmitterList.size(); ++i ) {
		ParticleEmitter* emitter = m_EmitterList[i];
		emitter->emitParticles(m_ParticleBuffer);
	}
}

// -------------------------------------------------------
// Add emitter
// -------------------------------------------------------
uint32 ParticleEmitterNode::addEmitter(ParticleEmitter* emitter) {
	uint32 id = m_IdCounter;
	emitter->setID(m_IdCounter);
	++m_IdCounter;
	m_EmitterList.push_back(emitter);
	return id;
}

// -------------------------------------------------------
// Remove emitter
// -------------------------------------------------------
void ParticleEmitterNode::removeEmitter(uint32 id) {
	EmitterList::iterator it = m_EmitterList.begin();
	while ( it != m_EmitterList.end()) {
		if ( (*it)->getID() == id ) {
			delete (*it);
			it = m_EmitterList.erase(it);
		}
		else {
			++it;
		}
	}
}

// -------------------------------------------------------
// Add particle affector
// -------------------------------------------------------
void ParticleEmitterNode::addParticleAffector(ParticleAffector* particleAffector) {
	m_AffectorList.push_back(particleAffector);
}

}
