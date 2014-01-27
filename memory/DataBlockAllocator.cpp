#include "DataBlockAllocator.h"
#include "..\utils\Log.h"

namespace ds {

DataBlockAllocator* gBlockMemory;

DataBlockAllocator::DataBlockAllocator(uint32 size) : m_BlockSize(size) {
	m_Current = new MemoryBlock(size);	
}


DataBlockAllocator::~DataBlockAllocator() {
	freeMemory();
}

void* DataBlockAllocator::alloc(uint32 size) {
	assert(size <= m_BlockSize);
	if( m_Current == 0 || size + m_Current->index > m_BlockSize ) {
		MemoryBlock* temp = new MemoryBlock(m_BlockSize);
		temp->next = m_Current;		
		m_Current = temp;
	}
	void *ret = m_Current->data + m_Current->index;
	++m_Current->elements;
	m_Current->index += (size + 3) & ~3; // dword align
	return ret;
}

void DataBlockAllocator::freeMemory() {
	MemoryBlock* current = m_Current;
	while ( current ) {
		MemoryBlock* tmp = current->next;
		delete current;
		current = tmp;
	}
}

void DataBlockAllocator::debug() {
	LOG(logINFO) << "---- DataBlockAllocator ----";
	MemoryBlock* current = m_Current;
	while ( current ) {
		LOG(logINFO) << "Size: " << current->index << " elements: " << current->elements;
		current = current->next;
	}
}

}
