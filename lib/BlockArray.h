#pragma once

class BlockArray {

public:
	BlockArray();
	~BlockArray();
	void init(int* sizes, int num);
	void* get_ptr(int index);
	bool resize(int new_size);
	int size() const {
		return _size;
	}
private:
	char* _data;
	int _size;
	int _capacity;
	int _sizes[16];
	int _indices[16];
	int _num_blocks;
};

