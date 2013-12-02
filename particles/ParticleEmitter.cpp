#include "ParticleEmitter.h"
#include "..\math\GameMath.h"
#include "..\utils\Log.h"

namespace ds {

ParticleEmitter::ParticleEmitter() : m_Timer(0) , m_Next(0) , m_Counter(0) {	
}

ParticleEmitter::ParticleEmitter(ParticleEmitterData* emitterData,ParticleData* particleData) 
	: m_Data(emitterData) , m_ParticleData(particleData) , m_Timer(0) , m_Next(0) , m_Counter(0) {	
}

ParticleEmitter::ParticleEmitter(ParticleData* particleData) 
	: m_ParticleData(particleData) , m_Timer(0) , m_Next(0) , m_Counter(0) {	
}

ParticleEmitter::~ParticleEmitter() {
	delete m_Data;
}

// -------------------------------------------------------
// Update particle emitter
// -------------------------------------------------------
void ParticleEmitter::update(ParticleBuffer* buffer,float elapsed) {
	int ejectCount = m_Data->ejectionCounter / 16;	
	if ( ejectCount > 0 ) {
		createParticles(buffer,ejectCount);	
	}	
}

// -------------------------------------------------------
// Prepare particle
// -------------------------------------------------------
Particle* ParticleEmitter::prepare(ParticleBuffer* buffer) {
	Particle* p = buffer->createParticle(m_ParticleData);
	assert(p != 0);
	p->rotationSpeed = math::random(m_ParticleData->minRotationSpeed,m_ParticleData->maxRotationSpeed);
	p->rotation = 0.0f;		
	p->radialVelocity = 1.0f;
	p->normal = Vec2(0,0);
	p->ttl = m_Data->ttl + math::random(-m_Data->ttlVariance,m_Data->ttlVariance);
	p->initialSize.x = m_ParticleData->initialSize.x + math::random(-m_Data->sizeXVariance,m_Data->sizeXVariance);
	p->initialSize.y = m_ParticleData->initialSize.y + math::random(-m_Data->sizeYVariance,m_Data->sizeYVariance);
	float vel = m_Data->velocity;
	if ( m_Data->velocityVariance != 0.0f ) {
		vel += math::random(-m_Data->velocityVariance,m_Data->velocityVariance);
	}		
	p->radialVelocity = vel;
	if ( m_ParticleData->random != 0.0f ) {
		p->random = math::random(-m_ParticleData->random,m_ParticleData->random);
	}
	else {
		p->random = 0.0f;
	}
	return p;
}

// -------------------------------------------------------
// Emit particles - triggered by particle emitter node
// -------------------------------------------------------
void ParticleEmitter::emitParticles(ParticleBuffer* buffer) {
	createParticles(buffer);
}

// -------------------------------------------------------
// StarEmitter
// -------------------------------------------------------
void StarEmitter::createParticles(ParticleBuffer* buffer,float forcedCount) {
	Vec2 pos;
	float xp = m_Position.x;
	float yp = m_Position.y;
	float angle = math::random(0.0f,360.0f);
	for ( int x = 0; x < 3; ++x ) {
		pos.x = m_Position.x;
		pos.y = m_Position.y;
		Particle* p = prepare(buffer);
		assert(p != 0);
		p->position = pos;				
		p->normal = vector::normalize(ds::math::getRadialVelocity(angle,p->radialVelocity));		
		angle += math::random(100.0f,140.0f);
	}	
}

// -------------------------------------------------------
// RingEmitter
// -------------------------------------------------------
RingEmitter::RingEmitter(ParticleEmitterData* emitterData,RingEmitterSettings* settings,ParticleData* particleData) 
	: ParticleEmitter(emitterData,particleData) , m_Settings(settings) {

}
void RingEmitter::createParticles(ParticleBuffer* buffer,float forcedCount) {
	Vec2 pos;
	float xp = m_Position.x;
	float yp = m_Position.y;
	float angle = m_Settings->startAngle + ds::math::random(-m_Settings->startVariance,m_Settings->startVariance);
	float step = 360.0f / static_cast<float>(m_Settings->count);
	for ( int x = 0; x < m_Settings->count; ++x ) {
		float ang = angle + math::random(-step * m_Settings->angleVariance,step * m_Settings->angleVariance);
		float rad = m_Settings->radius + math::random(-m_Settings->radiusVariance,m_Settings->radiusVariance);
		pos.x = m_Position.x + rad * cos(DEGTORAD(ang));
		pos.y = m_Position.y + rad * sin(DEGTORAD(ang));
		Particle* p = prepare(buffer);
		p->position = pos;		
		p->rotationSpeed = math::random(m_ParticleData->minRotationSpeed,m_ParticleData->maxRotationSpeed);
		p->normal = vector::normalize(math::getRadialVelocity(ang,p->radialVelocity));
		p->rotation = math::reflect(DEGTORAD(angle));			
		angle += step;
	}	
}

// -------------------------------------------------------
// ConeEmitter
// -------------------------------------------------------
void ConeEmitter::createParticles(ParticleBuffer* buffer,float forcedCount) {
	Vec2 pos;
	float xp = m_Position.x;
	float yp = m_Position.y;
	int count = m_Settings->count;
	if ( forcedCount != -1 ) {
		count = forcedCount;
	}
	float angle = m_Settings->startAngle;
	float diff = m_Settings->endAngle - m_Settings->startAngle;
	if ( diff < 0.0f ) {
		diff += 360.0f;
	}
	float step = diff / static_cast<float>(m_Settings->count);
	for ( int x = 0; x < count; ++x ) {
		float ang = angle + math::random(-step * m_Settings->angleVariance,step * m_Settings->angleVariance);
		float rad = m_Settings->radius + math::random(-m_Settings->radiusVariance,m_Settings->radiusVariance);
		pos.x = m_Position.x + rad * cos(DEGTORAD(ang));
		pos.y = m_Position.y + rad * sin(DEGTORAD(ang));
		Particle* p = prepare(buffer);
		assert(p != 0);
		p->position = pos;		
		p->rotationSpeed = math::random(m_ParticleData->minRotationSpeed,m_ParticleData->maxRotationSpeed);		
		p->normal = vector::normalize(math::getRadialVelocity(ang,p->radialVelocity));
		p->rotation = math::reflect(DEGTORAD(angle));
		if ( m_ParticleData->random != 0.0f ) {
			p->random = math::random(-m_ParticleData->random,m_ParticleData->random);
		}
		else {
			p->random = 0.0f;
		}
		angle += step;
		if ( angle > 360.0f ) {
			angle -= 360.0f;
		}
	}	
}


}