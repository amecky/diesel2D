#pragma once
#include "collection.h"
#include <string.h>

namespace array {

	// -----------------------------------------------------
	// returns size of the array
	// -----------------------------------------------------
	template<class T>uint32 size(Array<T>& ar) {
		return ar._size;
	}

	// -----------------------------------------------------
	// returns pointer to first element
	// -----------------------------------------------------
	template<class T>T* begin(Array<T>& ar) {
		return ar._data;
	}

	// -----------------------------------------------------
	// returns pointer to the last element
	// -----------------------------------------------------
	template<class T>T* end(Array<T>& ar) {
		return &ar._data[ar._size - 1];
	}

	// -----------------------------------------------------
	// adds an element to the end of the array
	// -----------------------------------------------------
	template<class T>void add(Array<T>& ar,const T& value) {
		if ( ar._size + 1 >= ar._capacity ) {
			set_capacity(ar,ar._capacity + BLOCK_SIZE);
		}
		ar._data[ar._size] = value;
		++ar._size;
	}

	// -----------------------------------------------------
	// erase an element at the given position
	// -----------------------------------------------------
	template<class T>void erase(Array<T>& ar,uint32 index) {
		if ( index < ar._size && ar._size > 0 ) {		
			for ( uint32 i = index; i < ar._capacity - 1; ++ i ) {
				ar._data[i] = ar._data[i+1];
			}
			--ar._size;
		}
	}

	// -----------------------------------------------------
	// clear entire array
	// -----------------------------------------------------
	template<class T>void clear(Array<T>& ar) {
		ar._size = 0;
		ar._capacity = 0;
	}

	// -------------------------------------------------------
	// erase element at give position and returns pointer 
	// to now current element at this position
	// -------------------------------------------------------
	template<class T>T* remove(uint32 index) {

	}
	// -----------------------------------------------------
	// sets new capacity if the new capacity is greater 
	// current capacity
	// -----------------------------------------------------
	template<class T>void set_capacity(Array<T>& ar,uint32 capacity) {
		if ( capacity > ar._capacity ) {
			T* tmp = new T[capacity];
			T* td = ar._data;
			memcpy(tmp,td,sizeof(T)*ar._size);
			ar._data = tmp;
			delete td;
			ar._capacity = capacity;
		}
	}


}