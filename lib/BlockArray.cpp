#include "BlockArray.h"
#include <string.h>
#include "..\memory\DefaultAllocator.h"

BlockArray::BlockArray() : size(0), capacity(0), data(0) , total_capacity(0) {
}


BlockArray::~BlockArray() {
	if (data != 0) {
		ds::gDefaultMemory->deallocate(data);
	}
}

void BlockArray::init(int* sizes, int num) {
	for (int i = 0; i < num; ++i) {
		_sizes[i] = sizes[i];
	}
	_num_blocks = num;
}

bool BlockArray::resize(int new_size) {
	if (new_size > capacity) {
		int total = 0;
		for (int i = 0; i < _num_blocks; ++i) {
			total += _sizes[i];
		}
		
		int sz = new_size * total;
		char* t = (char*)ds::gDefaultMemory->allocate(sz);
		if (data != 0) {
			int offset = 0;
			int old_offset = 0;
			for (int i = 0; i < _num_blocks; ++i) {
				memcpy(t + offset, data + old_offset, size * _sizes[i]);
				offset += new_size * _sizes[i];
				old_offset += capacity * _sizes[i];
			}
			ds::gDefaultMemory->deallocate(data);
		}
		capacity = new_size;
		_indices[0] = 0;
		for (int i = 1; i < _num_blocks; ++i) {
			_indices[i] = _indices[i - 1] + _sizes[i] * capacity;
		}
		data = t;
		return true;
	}
	return false;
}

void* BlockArray::get_ptr(int index) {
	return data + _indices[index];
}
