#include "ParticleBuffer.h"

namespace ds {

ParticleBuffer::ParticleBuffer(Renderer* renderer,uint32 maxParticles) : QuadBuffer(renderer,maxParticles) {
	m_ParticleHead = 0;
	m_FreeList = 0;
}

// -------------------------------------------------------
// Manage lifecycle of each particle
// -------------------------------------------------------
void ParticleBuffer::manageLifecyle(float elapsed) {
	Particle* current = m_ParticleHead;
	while ( current != 0 ) {
		current->timer += elapsed;
		if ( current->timer >= current->data->ttl ) {
			remove(current->index);
			current = freeParticle(current);
		}
		else {
			current = current->next;
		}
	}
}

// -------------------------------------------------------
// find particle
// -------------------------------------------------------
int ParticleBuffer::findParticle(Particle* particle) {
	int ret = -1;
	Particle* current = m_ParticleHead;
	int cnt = 0;
	while ( current != 0 ) {
		if ( current->index == particle->index ) {
			ret = cnt;
		}
		++cnt;
		current = current->next;
	}
	return ret;
}

// -------------------------------------------------------
// Size - returns number of active particles
// -------------------------------------------------------
uint32 ParticleBuffer::size() {
	uint32 cnt = 0;
	Particle* current = m_ParticleHead;
	while ( current != 0 ) {
		++cnt;
		current = current->next;
	}
	return cnt;
}
// ------------------------------------------------------------------
// Removes particle from the current list and it to to the free list
// ------------------------------------------------------------------
Particle* ParticleBuffer::freeParticle(Particle* particle) {	
	Particle* pp = m_ParticleHead;
	int idx = findParticle(particle);
	if ( idx == -1 ) {
		return 0;
	}
	else {
		if ( idx == 0 && m_ParticleHead->next != 0 ) {
			m_ParticleHead = m_ParticleHead->next;
			return m_ParticleHead;
		}	
		else if ( idx == 0 && m_ParticleHead->next == 0 ) {
			m_ParticleHead = 0;
			return 0;
		}
		Particle* current = m_ParticleHead;
		for ( int i = 0; i < (idx -1) ; ++i ) {
			current = current->next;
		}
		Particle* next = current->next;
		next = next->next;
		Particle* tmp = current->next;
		current->next = next;
		return current;
	}
	Particle* currentFree = m_FreeList;
	while ( currentFree != 0 ) {
		currentFree = currentFree->next;
	}
	if ( currentFree == 0 ) {
		m_FreeList = particle;
		m_FreeList->next = 0;
	}
	else {
		m_FreeList->next = particle;
		particle->next = 0;
	}
}

// -------------------------------------------------------
// Creates a new particle or use one from the free list
// -------------------------------------------------------
Particle* ParticleBuffer::createParticle(ParticleData* data) {
	if ( m_FreeList == 0 ) {
		Particle* tmp = new Particle();
		tmp->data = data;
		tmp->index = create(data->initialSize,data->initialSize,0.0f,Vec2(0,0),data->textureRect,1024.0f,tmp->color);
		tmp->sizeX = 1.0f;
		tmp->sizeY = 1.0f;
		tmp->rotation = 0.0f;
		tmp->next = 0;
		tmp->timer = 0.0f;
		if ( m_ParticleHead == 0 ) {
			m_ParticleHead = tmp;
			return tmp;
		}
		int cnt = 0;
		Particle* current = m_ParticleHead;
		while ( current->next != 0 ) {
			current = current->next;
			++cnt;
		}
		current->next = tmp;		
		return tmp;
	}
	else {
		Particle* tmp = m_FreeList;
		tmp->timer = 0.0f;		
		m_FreeList = m_FreeList->next;
		return tmp;
	}
}

// -------------------------------------------------------
// Moves all particles
// -------------------------------------------------------
void ParticleBuffer::moveParticles(float elapsed) {
	Vec2 pos;
	Particle* cp = m_ParticleHead;
	while ( cp != 0 ) {
		pos.x = cp->startPos.x + cp->timer * cp->velocity.x + 0.5f * cp->acceleration.x * cp->timer * cp->timer;
		pos.y = cp->startPos.y + cp->timer * cp->velocity.y + 0.5f * cp->acceleration.y * cp->timer * cp->timer;
		setColor(cp->index,cp->color);
		update(cp->index,cp->sizeX,cp->sizeY,cp->rotation,pos);
		cp = cp->next;
	}
}

// -------------------------------------------------------
// Returns the particle head
// -------------------------------------------------------
Particle* ParticleBuffer::getParticles() {
	return m_ParticleHead;
}

}

