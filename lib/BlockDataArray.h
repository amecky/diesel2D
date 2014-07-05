#pragma once
#include "DataArray.h"

namespace ds {

// -------------------------------------------------------
// BlockDataArray
// -------------------------------------------------------
template<class U,unsigned short BLOCK_SIZE,unsigned short MAX_OBJECTS = 256>
struct BlockDataArray {

	uint32 numObjects;
	Index indices[MAX_OBJECTS];
	uint32 nums[MAX_OBJECTS];
	U objects[BLOCK_SIZE * MAX_OBJECTS];
	ID internalIDs[MAX_OBJECTS];
	unsigned short free_enqueue;
	unsigned short free_dequeue;

	BlockDataArray() {
		clear();
	}

	// -------------------------------------------------------
	// clears the entire internal list
	// -------------------------------------------------------
	void clear() {
		numObjects = 0;
		for ( unsigned short i = 0; i < MAX_OBJECTS; ++i ) {
			indices[i].id = i;
			indices[i].next = i + 1;
			nums[i] = 0;
		}
		free_dequeue = 0;
		free_enqueue = MAX_OBJECTS - 1;
	}

	// -------------------------------------------------------
	// contains
	// -------------------------------------------------------
	const bool contains(ID id) const {
		const Index& in = indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}

	// -------------------------------------------------------
	// get value by ID and offset
	// -------------------------------------------------------
	U& get(ID id,uint32 offset) {
		assert(offset < BLOCK_SIZE);
		unsigned short index = indices[id & INDEX_MASK].index;
		assert(offset < nums[index]);
		return objects[index * BLOCK_SIZE + offset];
	}

	// -------------------------------------------------------
	// number of entries for ID
	// -------------------------------------------------------
	uint32 num(ID id) {
		unsigned short index = indices[id & INDEX_MASK].index;
		return nums[index];
	}

	// -------------------------------------------------------
	// Add value to specific index
	// -------------------------------------------------------
	void add(ID id,const U& v) {
		unsigned short index = indices[id & INDEX_MASK].index;
		int idx = index * BLOCK_SIZE;
		objects[idx + nums[index]] = v;
		++nums[index];
	}

	// -------------------------------------------------------
	// creates new one
	// -------------------------------------------------------
	ID create(const U& initialValue) {
		assert(numObjects != MAX_OBJECTS);
		Index &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = numObjects;
		nums[in.index] = 0;
		++numObjects;
		int ptr = in.index * BLOCK_SIZE;
		for ( int i =0 ; i < BLOCK_SIZE; ++i ) {
			objects[ptr + i] = initialValue;
		}
		internalIDs[in.index] = in.id;
		return in.id;
	}

	// -------------------------------------------------------
	// remove by ID
	// -------------------------------------------------------
	void remove(ID id) {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		int currentIdx = in.index * BLOCK_SIZE;		
		--numObjects;
		ID tmpID = internalIDs[numObjects];
		int nextIdx = numObjects * BLOCK_SIZE;
		for ( int i =0 ; i < BLOCK_SIZE; ++i ) {
			objects[currentIdx + i] = objects[nextIdx + i];
		}
		nums[in.index] = nums[numObjects];
		indices[tmpID & INDEX_MASK].index = in.index;
		in.index = USHRT_MAX;
		indices[free_enqueue].next = id & INDEX_MASK;
		free_enqueue = id & INDEX_MASK;
	}
};

}