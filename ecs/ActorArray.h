#pragma once
#include "..\lib\DataArray.h"
#include "..\math\Vector.h"

namespace ds {

const int MAX_OBJECTS = 1024;

struct ActorArray {

	unsigned int numObjects;
	Index indices[MAX_OBJECTS];
	ID internalIDs[MAX_OBJECTS];
	Vector2f positions[MAX_OBJECTS];
	unsigned short free_enqueue;
	unsigned short free_dequeue;

	ActorArray() {
		clear();
	}

	void clear() {
		numObjects = 0;
		for ( unsigned short i = 0; i < MAX_OBJECTS; ++i ) {
			indices[i].id = i;
			indices[i].next = i + 1;
		}
		free_dequeue = 0;
		free_enqueue = MAX_OBJECTS - 1;
	}

	const bool contains(ID id) const {
		const Index& in = indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}

	Vector2f& getPosition(ID id) {
		assert( id != UINT_MAX );
		unsigned short index = indices[id & INDEX_MASK].index;
		assert(index != USHRT_MAX);
		return positions[index];
	}

	ID add(const Vector2f& pos) {
		assert(numObjects != MAX_OBJECTS);
		Index &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = numObjects++;
		positions[in.index] = pos;
		internalIDs[in.index] = in.id;
		//U& o = objects[in.index];
		//o.id = in.id;
		return in.id;
	}

	void remove(ID id) {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		--numObjects;
		if ( numObjects > 0 ) {
		ID lastID = internalIDs[numObjects];
		Index& oldIndex = indices[lastID & INDEX_MASK];
		positions[in.index] = positions[oldIndex.index];
		//U& o = objects[in.index];
		//o = objects[--numObjects];
		
		indices[lastID & INDEX_MASK].index = in.index;
		in.index = USHRT_MAX;
		indices[free_enqueue].next = id & INDEX_MASK;
		free_enqueue = id & INDEX_MASK;
		}
	}
};

}