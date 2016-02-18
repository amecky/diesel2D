#pragma once
#include "..\dxstdafx.h"
#include <assert.h>
#include "..\math\GameMath.h"
#include "..\memory\DefaultAllocator.h"

namespace ds {

	template <typename TYPE> void Construct(void* dest) {
		new ((TYPE*)dest) TYPE;
	}

	template <typename TYPE> void Destruct(void* dest) {
		((TYPE*)dest)->~TYPE();
	}

	struct CharBuffer {

		char* data;
		int size;
		int capacity;
		int num;
		Allocator* _allocator;

		CharBuffer(Allocator* allocator = gDefaultMemory) : data(0), size(0), capacity(0), num(0) , _allocator(allocator) {}

		~CharBuffer() {
			if (data != 0) {
				//delete[] data;
				//_allocator->deallocate(data);
				DEALLOC(data);
			}
		}

		void* alloc(int sz) {
			if (size + sz > capacity) {
				int d = capacity * 2 + 8;
				if (d < sz) {
					d = sz * 2 + 8;
				}
				resize(d);
			}
			auto res = data + size;
			size += sz;
			int d = sz / 4;
			if (d == 0) {
				d = 1;
			}
			num += d;
			return res;
		}

		void resize(int newCap) {
			if (newCap > capacity) {
				//char* tmp = (char*)_allocator->allocate(newCap);
				char* tmp = (char*)ALLOC(newCap);
				if (data != 0) {
					memcpy(tmp, data, capacity);
					//_allocator->deallocate(data);
					DEALLOC(data);
				}
				capacity = newCap;
				data = tmp;
			}
		}

		void append(const char* s, int len) {
			if (size + len + 1 > capacity) {
				resize(capacity + len + 1 + 8);
			}
			const char* t = s;
			for (int i = 0; i < len; ++i) {
				data[size++] = *t;
				++t;
			}
			data[size++] = '\0';
		}

		void append(const char* s) {
			int len = strlen(s);
			if (size + len + 1 > capacity) {
				resize(capacity + len + 1 + 8);
			}
			const char* t = s;
			for (int i = 0; i < len; ++i) {
				data[size++] = *t;
				++t;
			}
			data[size++] = '\0';
		}
	};

	template<class T>
	class Array {

	public:
		Array(Allocator* allocator = gDefaultMemory) : _allocator(allocator), _size(0), _capacity(0), _data(0) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
		}

		explicit Array(uint32 size, Allocator* allocator = gDefaultMemory) : _allocator(allocator), _size(size), _capacity(size) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
			//_data = (uchar*)_allocator->allocate(size * _typeSize);
			_data = (uchar*)ALLOC(size * _typeSize);
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
				//_allocator->deallocate(_data);
				DEALLOC(_data);
			}
		}



		class iterator {
		public:
			typedef iterator self_type;
			typedef T value_type;
			typedef T& reference;
			typedef T* pointer;
			typedef int difference_type;
			iterator(pointer ptr) : ptr_(ptr), _index(0) { }
			self_type operator++() { self_type i = *this; ptr_++; ++_index; return i; }
			self_type operator++(int junk) { ptr_++; ++_index; return *this; }
			self_type operator+(int junk) { ptr_ += junk; _index += junk; return *this; }
			reference operator*() { return *ptr_; }
			pointer operator->() { return ptr_; }
			pointer ptr() { return ptr_; }
			bool operator==(const self_type& rhs) { return ptr_ == rhs.ptr_; }
			bool operator!=(const self_type& rhs) { return ptr_ != rhs.ptr_; }
			uint32 index() { return _index; }
		private:
			uint32 _index;
			pointer ptr_;
		};

		Array& operator=(const Array &other) {
			if (_size > 0) {
				resize(other._size);
				_size = other._size;
				_capacity = other._capacity;
				//_data = (uchar*)_allocator->allocate(_size * _typeSize);
				_data = (uchar*)ALLOC(_size * _typeSize);
				memcpy(_data, other._data, sizeof(T) * _size);
			}
			else {
				_size = other._size;
				_capacity = other._capacity;
				//_data = (uchar*)_allocator->allocate(_size * _typeSize);
				_data = (uchar*)ALLOC(_size * _typeSize);
				memcpy(_data, other._data, sizeof(T) * _size);
			}
			return *this;
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
			if (_size == 0) {
				return end();
			}
			assert(it.index() < _size);
			T* ptr = it.ptr();
			if (_destructor) {
				Destruct<T>(ptr);
			}
			if (_size - it.index() - 1 > 0) {
				uint32 d = _size - it.index() - 1;
				T* old = ptr + d;
				memcpy(ptr, old, d);
			}
			--_size;
			return iterator(ptr);
		}

		int remove_all(const T& t) {
			int cnt = 0;
			iterator it = begin();
			while (it != end()) {
				int d = end().ptr() - it.ptr();
				if ((*it) == t) {
					it = remove(it);
					++cnt;
				}
				else {
					++it;
				}
			}
			return cnt;
		}

		uint32 size() const {
			return _size;
		}

		uint32 capacity() const {
			return _capacity;
		}

		void destroy_all() {
			iterator it = begin();
			while (it != end()) {
				delete (*it);
				++it;
			}
			clear();
		}

		void clear() {
			if (_destructor) {
				T* ptr = _items;
				for (uint32 i = 0; i < _size; ++i) {
					Destruct<T>(ptr);
					++ptr;
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
			int delta = _size - position.index();
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

		void reserve(uint32 newCapacity) {
			grow(newCapacity);
		}

		void resize(uint32 newCapacity) {
			if (newCapacity > _capacity) {
				grow(newCapacity);
			}
			else if (newCapacity < _capacity) {
				shrink(newCapacity);
			}
		}
		
	private:
		void shrink(uint32 newCapacity) {
			if (newCapacity < _capacity) {
				if (_size > newCapacity) {
					uchar* ptr = _data + newCapacity * _typeSize;
					if (_destructor) {
						for (uint32 i = newCapacity; i < _size; ++i) {
							Destruct<T>(ptr);
							ptr += _typeSize;
						}
					}
					_size = newCapacity;
				}
				//uchar* newItems = (uchar*)malloc(newCapacity * _typeSize);
				//uchar* newItems = (uchar*)_allocator->allocate(newCapacity * _typeSize);
				uchar* newItems = (uchar*)ALLOC(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, newCapacity * _typeSize);
				}
				//free(_data);
				//_allocator->deallocate(_data);
				DEALLOC(_data);
				_data = newItems;
				_capacity = newCapacity;
			}
		}

		void grow(uint32 newCapacity) {
			if (newCapacity > _capacity) {
				//uchar* newItems = (uchar*)malloc(newCapacity * _typeSize);
				//uchar* newItems = (uchar*)_allocator->allocate(newCapacity * _typeSize);
				uchar* newItems = (uchar*)ALLOC(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, _size * _typeSize);
				}
				uchar* ptr = newItems + _size * _typeSize;
				if (_constructor) {
					for (uint32 i = _size; i < newCapacity; ++i) {
						Construct<T>(ptr);
						ptr += _typeSize;
					}
				}
				if (_data != 0) {
					//free(_data);
					//_allocator->deallocate(_data);
					DEALLOC(_data);
				}
				_data = newItems;
				_capacity = newCapacity;
			}
		}

		Array(const Array &other) {
			_typeSize = sizeof(T);
			_size = other._size;
			_capacity = other._capacity;
			//_data = (uchar*)malloc(_size * _typeSize);
			//_data = (uchar*)_allocator->allocate(_size * _typeSize);
			_data = (uchar*)ALLOC(_size * _typeSize);
			memccpy(_data, other._data, sizeof(T) * _size);
		}

		Allocator* _allocator;
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

	template<class T>
	class Stack {

	public:
		Stack(Allocator* allocator = gDefaultMemory) : _allocator(allocator) , _size(0) , _capacity(0) , _data(0) {
			_typeSize = sizeof(T);
			_constructor = !__has_trivial_constructor(T);
			_destructor = !__has_trivial_destructor(T);
		}

		~Stack() {
			if (_data != 0) {
				clear();
				//_allocator->deallocate(_data);
				DEALLOC(_data);
			}
		}

		void push(const T& t) {
			if (_size + 1 > _capacity) {
				grow(_capacity * 2 + 8);
			}
			_items[_size++] = t;
		}

		void pop() {
			if (_size > 0) {
				uchar* ptr = _data + (_size - 1) * _typeSize;
				Destruct<T>(ptr);
				--_size;
			}
		}

		const T& top() const {
			return _items[_size - 1];
		}

		T& top() {
			return _items[_size - 1];
		}

		uint32 size() {
			return _size;
		}

		bool empty() {
			return _size == 0;
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
	private:
		void grow(uint32 newCapacity) {
			if (newCapacity > _capacity) {
				//uchar* newItems = (uchar*)_allocator->allocate(newCapacity * _typeSize);
				uchar* newItems = (uchar*)ALLOC(newCapacity * _typeSize);
				if (_data != 0) {
					memcpy(newItems, _data, _size * _typeSize);
				}
				uchar* ptr = newItems + _size * _typeSize;
				if (_data != 0) {
					//_allocator->deallocate(_data);
					DEALLOC(_data);
				}
				_data = newItems;
				_capacity = newCapacity;
			}
		}
		Allocator* _allocator;
		uint32 _size;
		uint32 _capacity;
		bool _constructor;
		bool _destructor;
		uint32 _typeSize;
		union {
			uchar* _data;
			T* _items;
		};

	};

}