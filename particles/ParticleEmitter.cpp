#include "ParticleEmitter.h"

namespace ds {

ParticleEmitter::ParticleEmitter(ParticleEmitterData* emitterData) : m_Data(emitterData) , m_Timer(0) , m_Next(0) {	
	rand.seed(GetTickCount());
}


ParticleEmitter::~ParticleEmitter() {
	
}

// -------------------------------------------------------
// Update particle emitter
// -------------------------------------------------------
void ParticleEmitter::update(ParticleBuffer* buffer,float elapsed) {
	if ( m_Data->ejectionPeriod != 0 ) {
		float ms = elapsed * 1000.0f;
		m_Timer += static_cast<uint32>(ms);
		if ( m_Timer > m_Next ) {
			m_Timer = 0;
			m_Next = m_Data->ejectionPeriod;
			if ( m_Data->ejectionVariance != 0 ) {
				float rnd = static_cast<float>(m_Data->ejectionVariance);
				int diff = static_cast<int>(random(-rnd,rnd));
				m_Next += diff;
			}
			for ( size_t i = 0; i < m_ParticleDataList.size();++i ) {
				createParticles(buffer,m_ParticleDataList[i]);
			}
		}
	}
}

// -------------------------------------------------------
// Adds particle data to list
// -------------------------------------------------------
void ParticleEmitter::addParticleData(ParticleData* particleData) {
	m_ParticleDataList.push_back(particleData);
}

// -------------------------------------------------------
// Emit particles - triggered by particle emitter node
// -------------------------------------------------------
void ParticleEmitter::emitParticles(ParticleBuffer* buffer) {
	for ( size_t i = 0; i < m_ParticleDataList.size();++i ) {
		createParticles(buffer,m_ParticleDataList[i]);
	}
}

// -------------------------------------------------------
// StarEmitter
// -------------------------------------------------------
void StarEmitter::createParticles(ParticleBuffer* buffer,ParticleData* particleData) {
	Vec2 pos;
	float xp = m_Position.x;
	float yp = m_Position.y;
	float angle = random(0.0f,360.0f);
	for ( int x = 0; x < 3; ++x ) {
		pos.x = m_Position.x;
		pos.y = m_Position.y;
		Particle* p = buffer->createParticle(particleData);
		p->startPos = pos;		
		float vel = m_Data->velocity;
		if ( m_Data->velocityVariance != 0.0f ) {
			vel += random(-m_Data->velocityVariance,m_Data->velocityVariance);
		}
		p->velocity = ds::math::getRadialVelocity(angle,vel);
		p->acceleration = ds::Vec2(0,0);	
		p->color = particleData->startColor;
		angle += random(100.0f,140.0f);
	}	
}

}