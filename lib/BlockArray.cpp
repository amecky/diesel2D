#include "BlockArray.h"
#include <string.h>

BlockArray::BlockArray() : _size(0), _capacity(0), _data(0) {
}


BlockArray::~BlockArray() {
	if (_data != 0) {
		delete[] _data;
	}
}

void BlockArray::init(int* sizes, int num) {
	for (int i = 0; i < num; ++i) {
		_sizes[i] = sizes[i];
	}
	_num_blocks = num;
}

bool BlockArray::resize(int new_size) {
	if (new_size > _capacity) {
		int total = 0;
		for (int i = 0; i < _num_blocks; ++i) {
			total += _sizes[i];
		}
		
		int sz = new_size * total;
		char* data = new char[sz];
		if (_data != 0) {
			char* p = data;
			for (int i = 1; i < _num_blocks; ++i) {
				memcpy(p, _data + i * _capacity, _size * _sizes[i]);
			}
			delete[] _data;
		}
		_capacity = new_size;
		_indices[0] = 0;
		for (int i = 1; i < _num_blocks; ++i) {
			_indices[i] = _indices[i - 1] + _sizes[i] * _capacity;
		}
		_data = data;
		return true;
	}
	return false;
}

void* BlockArray::get_ptr(int index) {
	return _data + _indices[index];
}
