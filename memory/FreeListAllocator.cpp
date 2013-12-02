#include "FreeListAllocator.h"
#include "..\utils\Log.h"

namespace ds {

FreeListAllocator::FreeListAllocator(uint32 memSize) {
	m_Allocate = new char[memSize];
	m_Begin = m_Allocate;
	m_End = m_Begin + memSize;
	m_Free = m_Begin;	
	m_MemoryUsed = 0;
	m_NumAllocations = 0;
}


FreeListAllocator::~FreeListAllocator(void) {
	assert(m_MemoryUsed == 0);
	assert(m_NumAllocations == 0);
	delete m_Begin;
}

// -------------------------------------------------------
// Allocate
// -------------------------------------------------------
void * FreeListAllocator::allocate( uint32 size, uint32 align /*= DEFAULT_ALIGN */ ) {
	assert(size != 0);
	// assert alignment power 4
	assert(align % 4 == 0);
	size = ((size +3)/4)*4;
	char* p = m_Allocate;
	Header* h = (Header*)p;
	char* data = (char*)adjust(h,align);
	p = data + size;
	// Reached the end of the buffer, wrap around to the beginning.
	if (p > m_End) {
		LOG(logERROR) << "Allocator exhausted";
		return 0;
	}
	fill(h, data, p - (char *)h);
	m_Allocate = p;
	++m_NumAllocations;
	m_MemoryUsed += h->size;
	//LOG(logINFO) << "allocated " << size << " header size " << h->size;
	return data;
}

// -------------------------------------------------------
// Fill data
// -------------------------------------------------------
void FreeListAllocator::fill(Header *header, void *data, uint32 size) {
	header->size = size;
	uint32 *p = (uint32*)(header + 1);
	while (p < data) {
		*p++ = HEADER_PAD_VALUE;
	}
}

// -------------------------------------------------------
// Adjust pointer to point to data
// -------------------------------------------------------
void* FreeListAllocator::adjust(Header *header, uint32 align) {
	void *p = header + 1;
	uintptr_t pi = uintptr_t(p);
	const uint32 mod = pi % align;
	if (mod)
		pi += (align - mod);
	return (void *)pi;
}

// -------------------------------------------------------
// Deallocate
// -------------------------------------------------------
void FreeListAllocator::deallocate( void *p ) {
	
	if (!p) {
		return;
	}
	if (p < m_Begin || p >= m_End ) {
		LOG(logERROR) << "Deallocation outside our memory";
		return;
	}

	// Mark this slot as free
	//Header *h = header(p);

	uint32 *pd = (uint32*)p;
	while (pd[-1] == HEADER_PAD_VALUE)
		--pd;
	Header* h = (Header *)pd - 1;

	//LOGC(logINFO,"FreeListAllocator") << "deallocating : " << h->size;
	m_MemoryUsed -= h->size;
	assert((h->size & 0x80000000u) == 0);
	h->size = h->size | 0x80000000u;

	// Advance the free pointer past all free slots.
	while (m_Free != m_Allocate) {
		Header *h = (Header *)m_Free;
		if ((h->size & 0x80000000u) == 0)
			break;

		m_Free += h->size & 0x7fffffffu;
		if (m_Free == m_End) {
			LOG(logERROR) << "Buffer exhausted";
		}
	}

	--m_NumAllocations;
	
}

uint32 FreeListAllocator::allocated_size( void *p ) {
	return 0;
}

uint32 FreeListAllocator::total_allocated() {
	return m_MemoryUsed;
}


}