#pragma once
#include "..\dxstdafx.h"

namespace ds {

class DataBlockAllocator {

struct MemoryBlock {

	MemoryBlock* next;
	uchar* data;
	uint32 index;
	uint32 blockSize;
	uint32 elements;

	MemoryBlock(uint32 size) : index(0) , elements(0) , next(0) , blockSize(size) {
		data = new uchar[size];
	}
	~MemoryBlock() {
		delete[] data;
	}
};

public:
	DataBlockAllocator(uint32 size = 16376);
	~DataBlockAllocator();
	void* alloc(uint32 size);
	void freeMemory();
	void debug();
private:
	uint32 m_BlockSize;
	MemoryBlock* m_Current;
};

extern DataBlockAllocator* gBlockMemory;

}

