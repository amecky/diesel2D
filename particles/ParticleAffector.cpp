#include "ParticleAffector.h"
#include "..\math\GameMath.h"
#include <vector>
#include <string>
#include "..\utils\Log.h"

namespace ds {

// -------------------------------------------------------
// Wiggle affector - readCategory
// -------------------------------------------------------
void WiggleAffector::update(Particle* particle,float elapsed) {
	/*
	Particle* current = particle;	
	while ( current != 0 ) {
		Vec2 vel = current->velocity;
		Vec2 perp = Vec2(-vel.y,vel.x);
		perp = vector::normalize(perp);
		perp = perp * (sin(current->timer * m_Settings->amplitude) * m_Settings->radius) * math::random(-m_Settings->random,m_Settings->random);	
		vel += perp;
		vector::addScaled(current->position,vel,elapsed);
		current = current->next;
	}
	*/
}

}
