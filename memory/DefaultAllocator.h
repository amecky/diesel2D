#pragma once
#include "memory_types.h"
#include "..\io\ReportWriter.h"

namespace ds {

	struct MemoryInfo {

		uint32 allocated;
		uint32 blocks;

	};
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
	MemoryInfo get_info();
	void debug();
	void save(const ReportWriter& writer);
private:
	Header* _headers;
	int _num;
	int _header_capacity;
	char* _buffer;
	uint32 _capacity;
};

extern DefaultAllocator* gDefaultMemory;

}
