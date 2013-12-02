#include "BoxEmitter.h"

namespace ds {

void BoxEmitter::createParticles(ParticleBuffer* buffer,float forcedCount) {
	Vec2 pos;
	float xp = m_Position.x;
	float yp = m_Position.y;
	
	int stepsX = m_Settings->width / m_Settings->size;
	int stepsY = m_Settings->height / m_Settings->size;

	float cx = m_Position.x + static_cast<float>(m_Settings->width) * 0.5f;
	float cy = m_Position.y + static_cast<float>(m_Settings->height) * 0.5f;
	Vec2 center = Vec2(cx,cy);
	for ( int y = 0; y < stepsY; ++y ) {
		for ( int x = 0; x < stepsX; ++x ) {	
			pos.x = m_Position.x + x * m_Settings->size;
			pos.y = m_Position.y + y * m_Settings->size;
			Particle* p = prepare(buffer);
			assert(p != 0);
			p->position = pos;		
			p->rotationSpeed = math::random(m_ParticleData->minRotationSpeed,m_ParticleData->maxRotationSpeed);
			Vec2 d = pos - center;
			Vec2 dn = vector::normalize(d);
			float angle = math::getAngle(dn,Vec2(1,0)) + DEGTORAD(math::random(-m_Settings->angleVariance,m_Settings->angleVariance));
			p->rotation = angle;	
			angle = math::reflect(angle);			
			p->normal = vector::normalize(math::getRadialVelocity(RADTODEG(angle),p->radialVelocity));
		}
	}	
}

}