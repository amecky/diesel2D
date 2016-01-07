#pragma once
#include "memory_types.h"
//#include "..\lib\collection_types.h"
#include <vector>

namespace ds {

// -------------------------------------------------------
//  Header
// -------------------------------------------------------
struct Header {
	uint32 size;
	bool used;
	uint32 index;
	uint32 originalSize;

	Header() : size(0), used(false) , index(0) , originalSize(0) {}
};

// -------------------------------------------------------
// MallocAllocator
// -------------------------------------------------------
class DefaultAllocator : public Allocator {

public:
	DefaultAllocator(uint32 size = 4 * 1024 * 1024);
	~DefaultAllocator();
	void* allocate(uint32 size, uint32 align = 0 );
	void deallocate(void *p);
	uint32 allocated_size( void *p );
	uint32 total_allocated();
	void debug();
private:
	std::vector<Header> _headers;
	char* _buffer;
	uint32 _capacity;
};

extern DefaultAllocator* gDefaultMemory;

}
