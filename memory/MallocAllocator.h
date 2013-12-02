#pragma once
#include "memory_types.h"

namespace ds {

// -------------------------------------------------------
//  Header
// -------------------------------------------------------
struct Header {
	uint32 size;
};

// -------------------------------------------------------
// MallocAllocator
// -------------------------------------------------------
class MallocAllocator : public Allocator {

public:
	MallocAllocator();
	~MallocAllocator();
	void * allocate( uint32 size, uint32 align = DEFAULT_ALIGN );
	void deallocate( void *p );
	uint32 allocated_size( void *p );
	uint32 total_allocated();
private:
	uint32 m_TotalAllocated;
};

}
