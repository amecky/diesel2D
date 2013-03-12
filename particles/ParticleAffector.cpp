#include "ParticleAffector.h"

namespace ds {
// -------------------------------------------------------
// Size affector
// -------------------------------------------------------
SizeAffector::SizeAffector(float minX,float maxX,float minY,float maxY) {
	m_MinX = minX;
	m_MaxX = maxX;
	m_MinY = minY;
	m_MaxY = maxY;
}

void SizeAffector::update(Particle* particle,float elapsed) {
	Particle* current = particle;	
	while ( current != 0 ) {	
		ParticleData* data = current->data;
		float norm = current->timer / data->ttl;
		current->sizeX = m_MinX + ( 1.0f - norm ) * ( m_MaxX - m_MinX);//data->minSize + (1.0f - norm) * (data->maxSize - data->minSize);
		current->sizeY = m_MinY + ( 1.0f - norm ) * ( m_MaxY - m_MinY);
		current = current->next;
	}
}

// -------------------------------------------------------
// Color affector
// -------------------------------------------------------
void ColorAffector::update(Particle* particle,float elapsed) {
	Particle* current = particle;	
	Color fc;
	while ( current != 0 ) {	
		ParticleData* data = current->data;	
		Color sc = data->startColor;
		Color ec = data->endColor;
		float norm = current->timer / data->ttl;
		math::interpolateColor(sc,ec,norm,&fc);
		current->color = fc;
		current = current->next;
	}
}

void WiggleAffector::update(Particle* particle,float elapsed) {
	/*
	Vec2 pp = particle->;
	Vec2 np = pp;
	np.x += p->velocity.x * elapsed;
	np.y += p->velocity.y * elapsed;

	Vec2 diff = np - pp;
	Vec2 perp = Vec2(diff.y,-diff.x);
	perp.unit();
	float t = p->timer / p->ttl * DEGTORAD(360.0f);
	float offset = m_Radius * sin(p->angle + t * m_Amplitude);
	Vec2 sum = perp * offset;
	p->position.x += sum.x;
	p->position.y += sum.y;				
	*/
}

}
