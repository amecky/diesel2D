#pragma once
#include "memory_types.h"

namespace ds {

class FreeListAllocator : public Allocator {

struct Header {
	uint32 size;
};

public:
	FreeListAllocator(uint32 memSize);
	~FreeListAllocator(void);
	void * allocate( uint32 size, uint32 align = DEFAULT_ALIGN );
	void deallocate( void *p );
	uint32 allocated_size( void *p );
	uint32 total_allocated();
private:
	void* adjust(Header *header, uint32 align);
	void fill(Header *header, void *data, uint32 size);
	char* m_Begin;
	char* m_End;
	char* m_Allocate;
	char* m_Free;
	uint32 m_MemoryUsed;
	uint32 m_NumAllocations;
};

}

