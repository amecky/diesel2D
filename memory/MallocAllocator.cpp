#include "MallocAllocator.h"
#include "..\utils\Log.h"

namespace ds {

// Given a pointer to the header, returns a pointer to the data that follows it.
inline void *data_pointer(Header *header, uint32 align) {
	void *p = header + 1;
	return memory::align_forward(p, align);
}

// Given a pointer to the data, returns a pointer to the header before it.
inline Header *header(void *data) {
	uint32 *p = (uint32 *)data;
	while (p[-1] == HEADER_PAD_VALUE)
		--p;
	return (Header *)p - 1;
}

// Stores the size in the header and pads with HEADER_PAD_VALUE up to the
// data pointer.
inline void fill(Header *header, void *data, uint32 size) {
	header->size = size;
	uint32 *p = (uint32 *)(header + 1);
	while (p < data)
		*p++ = HEADER_PAD_VALUE;
}

MallocAllocator::MallocAllocator() : m_TotalAllocated(0) {
}


MallocAllocator::~MallocAllocator() {
	LOGC(logINFO,"MallocAllocator") << "Removing MallocAllocator - allocations: " << m_TotalAllocated;
	assert ( m_TotalAllocated == 0 );
}

void * MallocAllocator::allocate( uint32 size, uint32 align) {
	LOGC(logINFO,"MallocAllocator") << "Allocating size " << size << " align " << align;
	const uint32 ts = size + align + sizeof(Header);
	Header *h = (Header *)malloc(ts);
	void *p = data_pointer(h, align);
	fill(h, p, ts);
	m_TotalAllocated += ts;
	return p;
}

void MallocAllocator::deallocate( void *p ) {
	if (!p)
		return;

	Header *h = header(p);
	m_TotalAllocated -= h->size;
	free(h);
}

uint32 MallocAllocator::allocated_size( void *p ) {
	return header(p)->size;
}

uint32 MallocAllocator::total_allocated() {
	return m_TotalAllocated;
}

}