#include "EmitterMesh.h"

namespace ds {

// -------------------------------------------------------
// RingEmitterMesh - getPoint
// -------------------------------------------------------
void RingEmitterMesh::getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random) {
	float myAngle = m_Settings->startAngle + ds::math::random(-m_Settings->startVariance,m_Settings->startVariance);
	float step = 360.0f / static_cast<float>(total);
	int idx = index;
	if ( random ) {
		idx = math::random(0,total);
	}
	myAngle += step * idx;
	//float ang = angle + math::random(-step * m_Settings->angleVariance,step * m_Settings->angleVariance);
	float ang = myAngle + myAngle * math::random(-m_Settings->angleVariance,m_Settings->angleVariance);
	int radius = m_Settings->radius + math::random(-m_Settings->radiusVariance,m_Settings->radiusVariance);
	float rad = static_cast<float>(radius);
	position.x = rad * cos(DEGTORAD(ang));
	position.y = rad * sin(DEGTORAD(ang));	
	if ( m_Settings->normalVariance != 0.0f ) {
		ang = ang + ang * math::random(-m_Settings->normalVariance,m_Settings->normalVariance);
	}
	*angle = DEGTORAD(ang);
	normal = normalize(math::getRadialVelocity(ang,1.0f));
}

void PointEmitterMesh::getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random) {
	position = Vector2f(0,0);
	normal = Vector2f(0,0);
}

BoxEmitterMesh::BoxEmitterMesh(BoxEmitterSettings* settings) : EmitterMesh() , m_Settings(settings) {
	Vector2f pos;
	float xp = 0.0f;
	float yp = 0.0f;

	int stepsX = m_Settings->width / m_Settings->size + 1;
	int stepsY = m_Settings->height / m_Settings->size + 1;

	m_Max = stepsX * stepsY;
	m_Entries = new BoxEntry[stepsX * stepsY];

	float cx = static_cast<float>(m_Settings->width) * 0.5f;
	float cy = static_cast<float>(m_Settings->height) * 0.5f;
	Vector2f center = Vector2f(cx,cy);
	BoxEntry* current = m_Entries;
	for ( int y = 0; y < stepsY; ++y ) {
		for ( int x = 0; x < stepsX; ++x ) {	
			pos.x = x * m_Settings->size;
			pos.y = y * m_Settings->size;
			current->pos = pos;
			Vector2f d = pos - center;
			Vector2f dn = normalize(d);
			float angle = math::getAngle(dn,Vector2f(1,0)) + DEGTORAD(math::random(-m_Settings->angleVariance,m_Settings->angleVariance));			
			current->angle = angle;	
			angle = math::reflect(angle);								
			current->normal.x = cos(angle);
			current->normal.y = sin(angle);
			++current;
		}
	}	
}

void BoxEmitterMesh::getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random) {
	BoxEntry* current = m_Entries;
	int idx = index;
	if ( idx >= m_Max ) {
		idx -= m_Max;
	}
	current += idx;
	position = current->pos;
	normal = current->normal;
}

void ConeEmitterMesh::getPoint(uint32 index,uint32 total,Vector2f& position,Vector2f& normal,float* angle,bool random) {
	float myAngle = m_Settings->startAngle;
	float diff = m_Settings->endAngle - m_Settings->startAngle;
	if ( diff < 0.0f ) {
		diff += 360.0f;
	}
	float step = diff / static_cast<float>(total);
	myAngle += step * index;

	float ang = myAngle + math::random(-step * m_Settings->angleVariance,step * m_Settings->angleVariance);
	int radius = m_Settings->radius + math::random(-m_Settings->radiusVariance,m_Settings->radiusVariance);
	float rad = static_cast<float>(radius);
	position.x = rad * cos(DEGTORAD(ang));
	position.y = rad * sin(DEGTORAD(ang));
	*angle = DEGTORAD(ang);
	normal = normalize(math::getRadialVelocity(ang,1.0f));	
}


}