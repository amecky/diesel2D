#include "ParticleBuffer.h"
#include "..\math\vector.h"
#include "..\utils\Log.h"

namespace ds {

ParticleBuffer::ParticleBuffer(uint32 maxParticles) : m_Allocator(maxParticles) , m_MaxParticles(maxParticles) {
	LOG(logINFO) << "Creating new particle buffer with max particles: " << maxParticles;
	m_Head = 0;
	m_FreeList = 0;
	m_Counter = 0;
}

// -------------------------------------------------------
// Manage lifecycle of each particle
// -------------------------------------------------------
void ParticleBuffer::manageLifecyle(float elapsed) {
	Particle* p = m_Head;
	int cnt = 0;
	while ( p != 0 ) {
		p->timer += elapsed;
		assert(p->data != 0);
		if ( p->ttl > 0.0f && p->timer > p->ttl ) {
			Particle* tmp = p->next;
			freeParticle(p);
			p = tmp;
		}
		else {
			p = p->next;
		}
	}	
}

// -------------------------------------------------------
// Size - returns number of active particles
// -------------------------------------------------------
uint32 ParticleBuffer::size() {
	uint32 cnt = 0;
	Particle* p = m_Head;
	while ( p != 0 ) {
		++cnt;
		p = p->next;
	}
	return cnt;
}

uint32 ParticleBuffer::free() {
	uint32 cnt = 0;
	Particle* p = m_FreeList;
	while ( p != 0 ) {
		++cnt;
		p = p->next;
	}
	return cnt;
}
// ------------------------------------------------------------------
// Removes particle from the current list and it to to the free list
// ------------------------------------------------------------------
void ParticleBuffer::freeParticle(Particle* particle) {	
	if( particle->prev ) {
		particle->prev->next = particle->next;
	}
	else {
		// this handles the case that we delete the first node in the used list
		m_Head = particle->next;
	}

	if( particle->next ) {
		particle->next->prev = particle->prev;
	}

	// add to free list
	if( m_FreeList == 0 ) {
		// free list was empty
		m_FreeList = particle;
		particle->prev = 0;
		particle->next = 0;
	}
	else {
		// Add this node at the start of the free list
		m_FreeList->prev = particle;
		particle->next = m_FreeList;
		m_FreeList = particle;
	}
	
	--m_Counter;
}

// -------------------------------------------------------
// Creates a new particle or use one from the free list
// -------------------------------------------------------
Particle* ParticleBuffer::createParticle(ParticleData* data) {
	Particle* p = m_FreeList;
	if ( p == 0 ) {
		p = m_Allocator.alloc();
		if ( p == 0 ) {
			return 0;
		}
		p->prev = 0;
		p->next = m_Head;
	}
	else {
		m_FreeList = p->next;
	}
	// if the new node points to another free node then
	// change that nodes prev free pointer...
	if( p->next ) {
		p->next->prev = 0;
	}

	// node is now on the used list

	p->prev = 0; // the allocated node is always first in the list

	if( m_Head == 0 ) {
		p->next = 0; // no other nodes
	}
	else {
		m_Head->prev = p; // insert this at the head of the used list
		p->next = m_Head;
	}

	m_Head = p;	
	p->rotation = 0.0f;			
	p->timer = 0.0f;
	p->data = data;
	++m_Counter;
	//LOG(logINFO) << "PB - new size " << m_Counter << " size() " << size() << " free() " << free();
	return p;

}

// -------------------------------------------------------
// Returns the particle head
// -------------------------------------------------------
Particle* ParticleBuffer::getParticles() {
	return m_Head;
}

}

