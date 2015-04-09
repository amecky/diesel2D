#pragma once
#include "..\utils\Log.h"

template<class T>
class PriorityQueue {

public:
	PriorityQueue() :data(0), size(0), total(0) {}
	~PriorityQueue() {
		if (data != 0) {
			delete[] data;
		}
	}
	void push(const T& v) {
		if ((size + 1) >= total) {
			allocate(total * 2);
		}
		// find spot
		int idx = -1;
		for (int i = 0; i < size; ++i) {
			if (data[i].compare(v) >= 0 && idx == -1) {
				idx = i;
			}
		}
		if (idx == -1) {
			// insert at the end
			data[size++] = v;
		}
		else {
			// insert at position
			insertAt(idx, v);
			++size;
		}
	}
	T pop() {
		T t = data[0];
		remove(0);
		return t;
	}
	void remove(int idx) {
		if (idx < size) {
			for (int i = idx; i < size - 1; ++i) {
				data[i] = data[i + 1];
			}
			if (size > 0) {
				--size;
			}
		}
	}
	const T& at(int index) const {
		return data[index];
	}
	T& at(int index) {
		return data[index];
	}
	int indexOf(const T& v) {
		for (int i = 0; i < size; ++i) {
			if (data[i].isMatch(v)) {
				return i;
			}
		}
		return -1;
	}
	const T* get() const {
		return data;
	}
	const int num() const {
		return size;
	}
	void clear() {
		size = 0;
	}
	void debug() {
		for (int i = 0; i < size; ++i) {
			data[i].debug(i);
		}
	}
private:
	void insertAt(int idx, const T& v) {
		for (int i = size - 1; i >= idx; --i) {
			data[i + 1] = data[i];
		}
		data[idx] = v;
	}
	void allocate(int sz) {
		if (data == 0) {
			data = new T[32];
			total = 32;
			size = 0;
		}
		else {
			T* tmp = new T[sz];
			for (int i = 0; i < total; ++i) {
				tmp[i] = data[i];
			}
			total = sz;
			delete[] data;
			data = tmp;
		}
	}
	T* data;
	int size;
	int total;
};