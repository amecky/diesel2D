#pragma once
#include "memory_types.h"
#include "..\io\ReportWriter.h"
#include <vector>
// ds::gDefaultMemory->trace(size,__FILE__,__LINE__);
#ifdef DEBUG
#define ALLOC(size) ds::gDefaultMemory->allocate(size,__FILE__,__LINE__);
#else
#define ALLOC(size) ds::gDefaultMemory->allocate(size);
#endif
#define ALLOCA(size,align) ds::gDefaultMemory->allocate(size,align);
#define DEALLOC(ptr) do { ds::gDefaultMemory->deallocate(ptr);} while(0)

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

struct AllocInfo {
	//int nameIndex;
	char name[48];
	int line;
	int size;
	bool reuse;
};

// -------------------------------------------------------
// MallocAllocator
// -------------------------------------------------------
class DefaultAllocator : public Allocator {

typedef std::vector<AllocInfo> AllocInfos;

public:
	DefaultAllocator(uint32 size = 256 * 1024 * 1024);
	~DefaultAllocator();
	void* allocate(uint32 size, uint32 align = 0 );
	void* allocate(uint32 size, const char* file,int line);
	void deallocate(void *p);
	uint32 allocated_size( void *p );
	uint32 total_allocated();
	MemoryInfo get_info();
	void debug();
	void save(const ReportWriter& writer);
	void trace(int size,const char* file, int line);
private:
	Header* _headers;
	int _num;
	int _header_capacity;
	char* _buffer;
	uint32 _capacity;
	AllocInfos _infos;
};

extern DefaultAllocator* gDefaultMemory;

}
