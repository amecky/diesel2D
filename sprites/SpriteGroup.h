#pragma once
#include "..\lib\DataArray.h"
#include <Vector.h>
#include "..\math\math_types.h"
#include "..\utils\Color.h"

namespace ds {

template<int SIZE>
struct SpriteGroup {

	unsigned int numObjects;
	Index indices[SIZE];
	ID ids[SIZE];
	Vector2f positions[SIZE];
	Vector2f scales[SIZE];
	Vector4f uv[SIZE];
	Vector2f dim[SIZE];
	float rotations[SIZE];
	Color colors[SIZE];
	float timers[SIZE];
	float delays[SIZE];
	unsigned short free_enqueue;
	unsigned short free_dequeue;

	SpriteGroup() {
		clear();
	}

	void clear() {
		numObjects = 0;
		for ( unsigned short i = 0; i < SIZE; ++i ) {
			indices[i].id = i;
			indices[i].next = i + 1;
		}
		free_dequeue = 0;
		free_enqueue = SIZE - 1;
	}

	const bool contains(ID id) const {
		const Index& in = indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}

	void setDelay(ID id,float delay) {
		unsigned short idx = getIndex(id);
		delays[idx] = delay;
	}

	unsigned short getIndex(ID id) {
		assert( id != UINT_MAX );
		unsigned short index = indices[id & INDEX_MASK].index;
		assert(index != USHRT_MAX);
		return index;
	}

	void tick(float t) {
		for ( unsigned int i = 0; i < numObjects; ++i ) {
			if ( delays[i] > 0.0f ) {
				delays[i] -= t;
				if ( delays[i] < 0.0f ) {
					delays[i] = 0.0f;
				}
			}
			else {
				timers[i] += t;
			}
		}
	}
	
	ID add(const Vector2f& position,const Rect& r) {
		assert(numObjects != SIZE);
		Index &in = indices[free_dequeue];
		free_dequeue = in.next;
		in.index = numObjects++;
		ids[in.index] = in.id;
		positions[in.index] = position;
		uv[in.index] = math::getTextureCoordinates(r);
		rotations[in.index] = 0.0f;
		timers[in.index] = 0.0f;
		colors[in.index] = Color::WHITE;
		scales[in.index] = Vector2f(1,1);
		dim[in.index] = Vector2f(r.width(), r.height());
		return in.id;
	}

	void remove(ID id) {
		Index &in = indices[id & INDEX_MASK];
		assert(in.index != USHRT_MAX);
		ID currentID = ids[numObjects-1];
		Index& next = indices[currentID & INDEX_MASK];

		positions[in.index] = positions[next.index];
		uv[in.index] = uv[next.index];
		dim[in.index] = dim[next.index];
		colors[in.index] = colors[next.index];
		scales[in.index] = scales[next.index];
		rotations[in.index] = rotations[next.index];
		timers[in.index] = timers[next.index];
		--numObjects;
		indices[currentID & INDEX_MASK].index = in.index;
		in.index = USHRT_MAX;
		indices[free_enqueue].next = id & INDEX_MASK;
		free_enqueue = id & INDEX_MASK;
	}
};

}


