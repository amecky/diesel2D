#pragma once

struct BlockArray {

	char* data;
	int size;
	int capacity;
	int total_capacity;
	int _sizes[16];
	int _indices[16];
	int _num_blocks;

	BlockArray();

	~BlockArray();

	void init(int* sizes, int num);

	void* get_ptr(int index);

	bool resize(int new_size);
	
};

