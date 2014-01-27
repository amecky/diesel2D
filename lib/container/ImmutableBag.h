#pragma once

namespace ds {

template<class T>
class ImmutableBag {

public:
	virtual ~ImmutableBag(){};
	virtual const T& get(int index) const = 0;
	virtual const int capacity() const = 0;
	virtual const int size() const = 0;
	virtual const bool isEmpty() const = 0;	
};

template<class T>
class Bag : public ImmutableBag<T> {

public:
	Bag() : m_Data(0) , m_Size(32) , m_Count(0) {
		m_Data = new T[32];
	}
	Bag(int size) : m_Count(0) {
		m_Size = size;
		m_Data = new T[size];
	}
	virtual ~Bag() {
		delete[] m_Data;
	}
	const T& get(int index) const {
		assert(index >= 0 && index < m_Size );
		return m_Data[index];
	}
	void add(const T& t);
	const int capacity() const {
		return m_Size;
	}
	const int size() const {
		return m_Count;
	}
	const bool isEmpty() const {
		return m_Count == 0;
	}
private:
	void grow(int size);
	int m_Count;
	int m_Size;
	T* m_Data;
};

template<class T>
void Bag<T>::grow(int size) {
	T* tmp = new T[size];
	for ( int i = 0; i < m_Count; ++i ) {
		tmp[i] = m_Data[i];
	}
	delete[] m_Data;
	m_Size = size;
	m_Data = tmp;

}

template<class T>
void Bag<T>::add(const T& t) {
	m_Data[m_Count] = t;
	++m_Count;
	if ( m_Count >= m_Size ) {
		grow(m_Size*2);
	}
}

}

