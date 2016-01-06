#pragma once
#include "..\dxstdafx.h"
#include <assert.h>
#include "..\math\GameMath.h"

namespace ds {

	template <typename TYPE> void Construct(void* dest) {
		new ((TYPE*)dest) TYPE;
	}

	template <typename TYPE> void Destruct(void* dest) {
		((TYPE*)dest)->~TYPE();
	}

	template<class T>
	class Array {

	public:
		Array() : _size(0), _capacity(0), _data(0) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
		}

		explicit Array(uint32 size) : _size(size), _capacity(size) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
			_data = (uchar*)malloc(size * _typeSize);
			uchar* ptr = _data;
			if (_constructor) {
				for (uint32 i = 0; i < _size; ++i) {
					Construct<T>(ptr);
					ptr += _typeSize;
				}
			}
		}

		~Array() {
			if (_data != 0) {
				clear();
				free(_data);
			}
		}

		Array(const Array &other) {
			_typeSize = sizeof(T);
			_size = other._size;
			_capacity = other._capacity;
			_data = (uchar*)malloc(_size * _typeSize);
			memccpy(_data, other._data, sizeof(T) * _size);
		}

		class iterator {
			public:
				typedef iterator self_type;
				typedef T value_type;
				typedef T& reference;
				typedef T* pointer;
				typedef int difference_type;
				iterator(pointer ptr) : ptr_(ptr) , _index(0) { }
				self_type operator++() { self_type i = *this; ptr_++; ++_index; return i; }
				self_type operator++(int junk) { ptr_++; ++_index; return *this; }
				self_type operator+(int junk) { ptr_ += junk; _index += junk; return *this; }
				reference operator*() { return *ptr_; }
				pointer operator->() { return ptr_; }
				bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
				bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
				uint32 index() { return _index; }
			private:
				uint32 _index;
				pointer ptr_;
		};

		Array& operator=(const Array &other) {

		}

		iterator begin() {
			return iterator(_items);
		}

		iterator end() {
			return iterator(_items + _size);
		}

		void remove(uint32 index) {
			assert(index < _size);
			uchar* ptr = _data + _typeSize * index;
			if (_destructor) {
				Destruct<T>(ptr);
			}
			if (_size - index - 1 > 0) {
				uint32 d = _size - index - 1;
				uchar* old = ptr + _typeSize;
				memcpy(ptr, old, d * _typeSize);
			}
			--_size;
		}

		iterator remove(iterator it) {
			assert(it.index() < _size);
			uchar* ptr = _data + _typeSize * it.index();
			if (_destructor) {
				Destruct<T>(ptr);
			}
			if (_size - it.index() - 1 > 0) {
				uint32 d = _size - it.index() - 1;
				uchar* old = ptr + _typeSize;
				memcpy(ptr, old, d * _typeSize);
			}
			--_size;
			return iterator(_items + it.index());
		}

		uint32 size() const {
			return _size;
		}

		uint32 capacity() const {
			return _capacity;
		}

		void clear() {
			if (_destructor) {
				uchar* ptr = _data;
				for (uint32 i = 0; i < _size; ++i) {
					Destruct<T>(ptr);
					ptr += _typeSize;
				}
			}
			_size = 0;
		}

		T& operator[](uint32 i) {
			assert(i < _size);
			return _items[i];
		}

		const T& operator[](uint32 i) const {
			assert(i < _size);
			return _items[i];
		}

		T& at(uint32 i) {
			assert(i < _size);
			return _items[i];
		}

		const T& at(uint32 i) const {
			assert(i < _size);
			return _items[i];
		}

		void push_back(const T& t) {
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}
			_items[_size++] = t;
		}

		iterator insert(iterator position, const T& t) {
			return insert(position, 1, t);
		}

		iterator insert(iterator position, uint32 n, const T& t) {
			assert(position.index() < _size);
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}			
			uint32 delta = _size - position.index() - 1;
			if (delta > 0) {
				uchar* ptr = _data + position.index() * _typeSize;
				uchar* next = ptr + _typeSize;
				memcpy(next, ptr, delta * _typeSize);
			}
			++_size;
			_items[position.index()] = t;
			return iterator(_items + position.index());
		}

		void pop_back() {
			if (_size > 0) {
				if (_destructor) {
					uchar* ptr = _data + (_size - 1) * _typeSize;
					Destruct<T>(ptr);
				}
				--_size;
			}
		}

		const T* data() const {
			return _items;
		}

		const T& front() const {
			assert(_size > 0);
			return _items[0];
		}

		T& front() {
			assert(_size > 0);
			return _items[0];
		}

		const T& back() const {
			assert(_size > 0);
			return _items[_size - 1];
		}

		T& back() {
			assert(_size > 0);
			return _items[_size - 1];
		}
		
		bool empty() const {
			return _size == 0;
		}

		void reserve(int newCapacity) {

		}

		void resize(int newCapacity) {

		}
	private:
		void grow(uint32 newCapacity) {
			if (newCapacity > _capacity) {
				uchar* newItems = (uchar*)malloc(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, _size * _typeSize);
				}
				free(_data);
				_data = newItems;
				_capacity = newCapacity;
			}
		}
		bool _constructor;
		bool _destructor;
		uint32 _typeSize;
		uint32 _size;
		uint32 _capacity;
		union {
			uchar* _data;
			T* _items;
		};
	};

}