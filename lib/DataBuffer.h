#pragma once
#include "..\dxstdafx.h"

namespace ds {

// -------------------------------------------------------
// DataIndex
// -------------------------------------------------------
struct DataIndex {
	uint32 id;
	uint32 index;
	uint32 next;
};

// -------------------------------------------------------
// DataBuffer
// -------------------------------------------------------
template<class T>
class DataBuffer {

public:
	DataBuffer(uint32 maxElements);
	~DataBuffer();
	uint32 add(const T& t);
	const uint32 size() const {
		return m_Count;
	}
private:
	char* m_Data;
	DataIndex* m_Indices;
	int m_FreeBegin;
	uint32 m_MaxElements;
	int m_ElementSize;
	uint32 m_Count;
};

template<class T>
inline DataBuffer<T>::DataBuffer(uint32 maxElements) : m_MaxElements(maxElements) , m_Count(0) {
	m_ElementSize = sizeof(T); 
	//m_Data = new char[m_ElementSize * m_MaxElements];
	//m_Indices = new DataIndex[m_MaxElements];
	m_Data = (char*)ALLOC(m_ElementSize * m_MaxElements);
	m_Indices = (char*)ALLOC(m_MaxElements);
	for ( uint32 i = 0; i < m_MaxElements; ++i ) {
		m_Indices[i].id = i;
		m_Indices[i].next = i + 1;
	}
	m_FreeBegin = 0;
}

template<class T>
inline DataBuffer<T>::~DataBuffer() {
	DEALLOC(m_Indices);
	DEALLOC(m_Data);
}

template<class T>
inline uint32 DataBuffer<T>::add(const T& t) {
	DataIndex& in = m_Indices[m_FreeBegin];
	m_FreeBegin = in.next;
	in.index = m_Count;
	char* data = m_Data + in.index * m_ElementSize;
	*(T*)data = t;
	++m_Count;
	return in.id;
}

}