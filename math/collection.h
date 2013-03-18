#pragma once

typedef unsigned int uint32;
const uint32 INITIAL_SIZE = 16;
const uint32 BLOCK_SIZE = 16;

template<class T>
struct Array {
	Array();
	Array(uint32 initialBlocks);
	T &operator[](uint32 i);
    const T &operator[](uint32 i) const;
	uint32 _size;
	uint32 _capacity;
	T* _data;
};

// -----------------------------------------------------
// Creating new empty array
// -----------------------------------------------------
template<class T>
inline Array<T>::Array() {
	_data = new T[INITIAL_SIZE];
	_size = 0;
	_capacity = INITIAL_SIZE;
}

// -----------------------------------------------------
// New array using initial block size
// -----------------------------------------------------
template<class T>
inline Array<T>::Array(uint32 initialBlocks) {
	_data = new T[initialBlocks * BLOCK_SIZE];
	_size = 0;
	_capacity = initialBlocks * BLOCK_SIZE;
}

// -----------------------------------------------------
// operator []
// -----------------------------------------------------
template<class T>
inline T& Array<T>::operator[](uint32 index) {
	return _data[index];
}

// -----------------------------------------------------
// const operator[]
// -----------------------------------------------------
template<class T>
inline const T& Array<T>::operator[](uint32 index) const {
	return _data[index];
}


