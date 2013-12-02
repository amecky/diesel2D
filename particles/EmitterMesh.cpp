#include "EmitterMesh.h"

namespace ds {

// -------------------------------------------------------
// RingEmitterMesh - getPoint
// -------------------------------------------------------
void RingEmitterMesh::getPoint(uint32 index,uint32 total,Vec2& position,Vec2& normal) {
	float angle = m_Settings->startAngle + ds::math::random(-m_Settings->startVariance,m_Settings->startVariance);
	float step = 360.0f / static_cast<float>(total);
	angle += step * index;

	float ang = angle + math::random(-step * m_Settings->angleVariance,step * m_Settings->angleVariance);
	float rad = m_Settings->radius + math::random(-m_Settings->radiusVariance,m_Settings->radiusVariance);
	position.x = rad * cos(DEGTORAD(ang));
	position.y = rad * sin(DEGTORAD(ang));		
	normal = vector::normalize(math::getRadialVelocity(ang,1.0f));
}

void PointEmitterMesh::getPoint(uint32 index,uint32 total,Vec2& position,Vec2& normal) {
	position = ds::Vec2(0,0);
	normal = ds::Vec2(0,0);
}

BoxEmitterMesh::BoxEmitterMesh(BoxEmitterSettings* settings) : EmitterMesh() , m_Settings(settings) {
	Vec2 pos;
	float xp = 0.0f;
	float yp = 0.0f;

	int stepsX = m_Settings->width / m_Settings->size;
	int stepsY = m_Settings->height / m_Settings->size;

	m_Max = stepsX * stepsY;
	m_Entries = new BoxEntry[stepsX * stepsY];

	float cx = static_cast<float>(m_Settings->width) * 0.5f;
	float cy = static_cast<float>(m_Settings->height) * 0.5f;
	Vec2 center = Vec2(cx,cy);
	BoxEntry* current = m_Entries;
	for ( int y = 0; y < stepsY; ++y ) {
		for ( int x = 0; x < stepsX; ++x ) {	
			pos.x = x * m_Settings->size;
			pos.y = y * m_Settings->size;
			
			current->pos = pos;
			
			Vec2 d = pos - center;
			Vec2 dn = vector::normalize(d);
			float angle = math::getAngle(dn,Vec2(1,0)) + DEGTORAD(math::random(-m_Settings->angleVariance,m_Settings->angleVariance));
			current->angle = angle;	
			angle = math::reflect(angle);			
			current->normal = vector::normalize(math::getRadialVelocity(RADTODEG(angle),1.0f));
			++current;
		}
	}	
}

void BoxEmitterMesh::getPoint(uint32 index,uint32 total,Vec2& position,Vec2& normal) {
	BoxEntry* current = m_Entries;
	int idx = index;
	if ( idx >= m_Max ) {
		idx -= m_Max;
	}
	current += idx;
	position = current->pos;
	normal = current->normal;
}


}