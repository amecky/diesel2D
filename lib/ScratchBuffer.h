#pragma once
#include "..\utils\Log.h"
#include <assert.h>

namespace ds {

// -------------------------------------------------------
// ScratchBuffer
// -------------------------------------------------------
template<class T>
class ScratchBuffer {

public:
	ScratchBuffer(int maxElements);
	~ScratchBuffer();
	const int size() const;
	bool add(const T& t);
	void clear() {
		m_Index = 0;
	}
	T* operator[] (int index) const;
	const T& get(int index) const;
private:
	int m_Index;
	int m_MaxElements;
	char* m_Data;
	int m_ElementSize;
};

// -------------------------------------------------------
// 
// -------------------------------------------------------
template<class T>
inline ScratchBuffer<T>::ScratchBuffer(int maxElements) : m_MaxElements(maxElements) , m_Index(0) {
	m_ElementSize = sizeof(T);	
	m_Data = new char[m_ElementSize * m_MaxElements];
}

// -------------------------------------------------------
// 
// -------------------------------------------------------
template<class T>
inline ScratchBuffer<T>::~ScratchBuffer() {
	delete[] m_Data;
}

// -------------------------------------------------------
// Size
// -------------------------------------------------------
template<class T>
inline const int ScratchBuffer<T>::size() const {
	return m_Index;
}

// -------------------------------------------------------
// Add
// -------------------------------------------------------
template<class T>
inline bool ScratchBuffer<T>::add(const T& t) {
	if ( m_Index < m_MaxElements ) {
		char* data = m_Data + m_Index * m_ElementSize;
		*(T*)data = t;
		++m_Index;
		return true;
	}
	return false;
}

// -------------------------------------------------------
// operator []
// -------------------------------------------------------
template<class T>
inline T* ScratchBuffer<T>::operator[] (int index) const {
	if ( index >= 0 && index < m_MaxElements ) {
		char* data = m_Data + index * m_ElementSize;
		return (T*)data;
	}
	return 0;
}

// -------------------------------------------------------
// get
// -------------------------------------------------------
template<class T>
inline const T& ScratchBuffer<T>::get (int index) const {
	assert(index >= 0);
	assert(index < m_MaxElements);
	char* data = m_Data + index * m_ElementSize;
	return *(T*)data;
}

}