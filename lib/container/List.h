#pragma once
#include "..\..\dxstdafx.h"

namespace ds {

template<class type>
class List {

public:
	List(uint32 granularity = 32);
	~List();
	void resize(uint32 newSize);
	uint32 size() const;
	uint32 num() const;
	uint32 append(const type& t);
	uint32 append(const List<type>& list);
	type& operator[](uint32 index);
	const type& operator[](uint32 index) const;
	void clear();
	void insertAt(uint32 index,const type& t);
	void deleteContents();
	int find(const type& t);
	bool removeAt(uint32 index);
	void assureSize(uint32 addon);
private:
	uint32 m_Granularity;
	uint32 m_Size;
	uint32 m_Num;
	type* m_Data;
};

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline List<type>::List(uint32 granularity) {
	m_Granularity = granularity;
	m_Data = 0;
	m_Size = 0;
	m_Num = 0;
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline List<type>::~List() {
	clear();
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline uint32 List<type>::size() const {
	return m_Size;
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline uint32 List<type>::num() const {
	return m_Num;
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline void List<type>::resize(uint32 newSize) {	
	//assert(newSize < 1024);
	// free up the list if no data is being reserved
	if ( newSize == 0 ) {
		clear();
		return;
	}
	if ( newSize == m_Size ) {
		return;
	}
	type* temp = m_Data;
	m_Size = newSize;
	if ( m_Size < m_Num ) {
		m_Num = m_Size;
	}
	// copy the old list into our new one
	m_Data = new type[m_Size];
	if ( temp != 0 ) {
		for(uint32 i = 0; i < m_Num; ++i ) {
			m_Data[i] = temp[i];
		}
	}
	// delete the old list if it exists
	if ( temp != 0 ) {
		delete[] temp;
	}
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline void List<type>::assureSize(uint32 addon) {
	uint32 required = m_Num + addon;
	// we need more than we have
	if ( required > m_Size ) {		
		resize(required);
	}
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline type& List<type>::operator[](uint32 index) {
	//assert(index < m_Num);
	return m_Data[index];
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline const type& List<type>::operator[](uint32 index) const {
	//assert(index < m_Num);
	return m_Data[index];
}


// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline uint32 List<type>::append(const type& obj) {
	if ( m_Data == 0 ) {
		resize(m_Granularity);
	}
	if ( m_Num == m_Size ) {		
		uint32 newsize = m_Size + m_Granularity;
		resize( newsize - newsize % m_Granularity );
	}
	m_Data[m_Num] = obj;
	++m_Num;
	return m_Num - 1;
}

// ----------------------------------------------------
// Inserts element at given index. If keepSize is true
// it will not grow the list of the new element would
// exceed the size
// ----------------------------------------------------
template<class type>
inline void List<type>::insertAt(uint32 index,const type& t) {
	if ( m_Num == m_Size ) {		
		uint32 newsize = m_Size + m_Granularity;
		resize( newsize - newsize % m_Granularity );	
	}
	if ( m_Num > 0 ) {
		append(m_Data[m_Num-1]);
	}
	for ( uint32 i = m_Num; i > index ; --i ) {
		m_Data[i] = m_Data[i-1];
	}
	m_Data[index] = t;
}
// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline uint32 List<type>::append(const List<type>& list) {
	if ( m_Data == 0 ) {
		resize(m_Granularity);
	}
	resize(list.num());
	for ( uint32 i = 0; i < list.num(); ++i ) {
		append(list[i]);
	}
	return m_Num-1;
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline void List<type>::clear() {	
	if ( m_Data != 0 ) {
		delete[] m_Data;
	}
	m_Data = 0;
	m_Num = 0;
	m_Size = 0;
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline void List<type>::deleteContents() {
	for( uint32 i = 0; i < m_Num; i++ ) {
		delete m_Data[i];
		m_Data[i] = 0;
	}
	clear();	
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template< class type >
inline bool List<type>::removeAt(uint32 index) {
	assert( m_Data != 0 );
	//assert( index >= 0 );
	assert( index < m_Num );
	if ( index >= m_Num ) {
		return false;
	}
	--m_Num;
	for( uint32 i = index; i < m_Num; ++i ) {
		m_Data[i] = m_Data[i + 1];
	}
	return true;
}

// ----------------------------------------------------
//
// ----------------------------------------------------
template<class type>
inline int List<type>::find(const type& t) {
	for( int i = 0; i < m_Num; i++ ) {
		if ( m_Data[i] == t ) {
			return i;
		}
	}
	return -1;
}

}
