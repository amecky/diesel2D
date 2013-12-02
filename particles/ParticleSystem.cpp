#include "ParticleSystem.h"
#include "..\utils\PlainTextReader.h"

namespace ds {
// -------------------------------------------------------
// 
// -------------------------------------------------------
ParticleSystem::ParticleSystem(Renderer* renderer,uint32 maxQuads,int textureID,int blendState)
	: m_IdCounter(0) , m_SizePath(0) , m_RadialVelocityPath(0) , m_RotationPath(0) {
	m_ParticleBuffer = new ParticleBuffer(maxQuads);
	m_SpriteBatch = new SpriteBatch(renderer,maxQuads,textureID);
	if ( blendState != -1 ) {
		m_SpriteBatch->setBlendState(blendState);
	}
	m_Camera = 0;
	m_TextureID = textureID;
	m_ColorPath = new ColorPath;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void ParticleSystem::draw() {
	m_SpriteBatch->begin();
	Color color = Color::WHITE;
	Vec2 size(0,0);
	Particle* p = m_ParticleBuffer->getParticles();
	while ( p != 0 ) {
		Vec2 pp = p->position;
		if ( m_Camera != 0 ) {
			pp = m_Camera->transform(pp);
		}
		float sizeX = p->initialSize.x;
		float sizeY = p->initialSize.y;
		float norm = p->timer / p->ttl;
		m_ColorPath->update(norm,&color);	
		if ( m_SizePath != 0 ) {
			m_SizePath->update(norm,&size);
			sizeX *= size.x;
			sizeY *= size.y;
		}
		m_SpriteBatch->draw(pp,m_ParticleData->textureRect,p->rotation,sizeX,sizeY,color);
		p = p->next;
	}
	m_SpriteBatch->end();
}

// -------------------------------------------------------
// Updates particle buffer and emitter
// -------------------------------------------------------
void ParticleSystem::update(float elapsed) {
	m_ParticleBuffer->manageLifecyle(elapsed);	
	m_Emitter->update(m_ParticleBuffer,elapsed);		
	Particle* current = m_ParticleBuffer->getParticles();	
	Vec2 size(0,0);
	float radialVelocity = 0.0f;
	float rotationSpeed = 0.0f;
	while ( current != 0 ) {	
		float norm = current->timer / current->ttl;
		radialVelocity = current->radialVelocity;		
		if ( m_RadialVelocityPath != 0 ) {
			m_RadialVelocityPath->update(norm,&radialVelocity);
			radialVelocity *= current->radialVelocity;
		}
		Vec2 velocity = current->normal * radialVelocity;
		if ( m_RotationPath != 0 ) {
			m_RotationPath->update(norm,&rotationSpeed);
			if ( current->random > 0.0f ) {				
				current->rotation -= elapsed * DEGTORAD(rotationSpeed) * current->random;
			}
			else {
				current->rotation += elapsed * DEGTORAD(rotationSpeed) * current->random;
			}
			float angle = math::reflect(current->rotation);
			velocity = math::getRadialVelocity(RADTODEG(angle),radialVelocity);
		}
		// move particle		
		vector::addScaled(current->position,velocity,elapsed);
		current = current->next;
	}
}

// -------------------------------------------------------
// Emit particles - triggered from outside
// -------------------------------------------------------
void ParticleSystem::emitParticles() {
	m_Emitter->emitParticles(m_ParticleBuffer);
}

// -------------------------------------------------------
// Add emitter
// -------------------------------------------------------
void ParticleSystem::setEmitter(ParticleEmitter* emitter) {
	m_Emitter = emitter;
}

// -------------------------------------------------------
// Set emitter position
// -------------------------------------------------------
void ParticleSystem::setEmitterPosition(const Vec2& pos) {
	m_Emitter->setPosition(pos);
}

void ParticleSystem::resetEmitterCounter() {
	m_Emitter->resetCounter();
}

// -------------------------------------------------------
// Start
// -------------------------------------------------------
void  ParticleSystem::start(const Vec2& startPos) {
	resetEmitterCounter();
	setEmitterPosition(startPos);
	emitParticles();
}

// -------------------------------------------------------
// Stop
// -------------------------------------------------------
void  ParticleSystem::stop() {
	resetEmitterCounter();
	// FIXME: clear all particles
}

// -------------------------------------------------------
// Set ParticleData
// -------------------------------------------------------
void ParticleSystem::setParticleData(ParticleData* particleData) {
	m_ParticleData = particleData;
	LOGC(logINFO,"ParticleSystem") << "Setting particle data";
	m_ColorPath->reset();
	m_ColorPath->add(0.0f,particleData->startColor);
	m_ColorPath->add(1.0f,particleData->endColor);	
}

}
