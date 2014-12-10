#pragma once
// -------------------------------------------------------
// dVector
// -------------------------------------------------------
template<class T>
class dVector {

public:
	typedef typename std::size_t size_type;
	typedef T* iterator;
	typedef T* pointer;
	dVector() {
		m_Size = 0;
		m_Data = 0;
		m_Capacity = 0;
	}
	explicit dVector(size_type initialSize) {
		grow(initialSize);
	}
	~dVector() {
		clear();
		delete[] m_Data;
	}
	const size_type size() const {
		return m_Size;
	}
	const size_type capacity() const {
		return m_Capacity;
	}    
	void clear() {
		deleteRange(m_Data,m_Data + m_Size);
		m_Size = 0;
	}
	// erase
	pointer erase(pointer iterator) {
		if ( iterator < m_Data + m_Size ) {            
			deleteRange(iterator, iterator + 1);
			copyRange(iterator +1, m_Data + m_Size,iterator);
			--m_Size;
			if ( m_Size < 0 ) {
				m_Size = 0;
			}
			return iterator;
		}
		else {
			return end();
		}
	}
	// insert at
	void insertAt(size_type index,const T& value) {
		pointer ptr = m_Data + index;
		if ( (m_Size +1 ) >= m_Capacity ) {
			grow(16);
		}
		copyRange(ptr,m_Data + m_Size, ptr + 1);
		m_Data[index] = value;
		++m_Size;
	}
	// get
	const T& get(size_type idx) const {
		return m_Data[idx];
	}
	T& get(size_type idx) {
		return m_Data[idx];
	}
	// set
	void set(size_type index,const T& value) {
		m_Data[index] = value;
	}
	int create(){
		T t;
		int id = m_Size;
		add(t);
		return id;
	}
	void add(const T& value) {
		if ( m_Size != m_Capacity ) {
			//m_Data[m_Size] = value;
			new((void*)(m_Data + m_Size)) T(value);
			++m_Size;
		}
		else {
			grow(16);
			//m_Data[m_Size] = value;
			new((void*)(m_Data + m_Size)) T(value);
			++m_Size;
		}
	}
	const bool isEmpty() const {
		return m_Size == 0;
	}    
	const T& operator[] (size_type index) const {
		return m_Data[index];
	}
	T& operator[] (size_type index) {
		return m_Data[index];
	}
	pointer begin() const {
		return m_Data;
	}
	pointer end() const {
		return m_Data + m_Size;
	}
private:
	void grow(size_type newSize) {
		if ( m_Data == 0 ) {
			m_Data = new T[newSize];
			m_Capacity = newSize;
			m_Size = 0;
		}
		else {
			size_type newCapacity = m_Capacity ? m_Capacity * 3 : 8;
			T* newData = new T[newCapacity];
			copyRange(m_Data, m_Data + m_Size, newData);
			deleteRange(m_Data, m_Data + m_Size);
			delete[] m_Data;
			m_Data = newData;
			m_Capacity = newCapacity;
		}
	}
	void copyRange(pointer begin, pointer end, pointer dest) {
		while(begin != end) {
			new((void*)dest) T(*begin);
			++begin;
			++dest;
		}
	}
	void deleteRange(pointer begin, pointer end) {
		while(begin != end) {
			begin->~T();
			++begin;
		}
	}
	pointer m_Data;
	size_type m_Size;
	size_type m_Capacity;
};
