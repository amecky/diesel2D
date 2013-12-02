#pragma once

namespace ds {

template<class Type>
class FSAllocator {

struct Chunk {
	Type type;
	Chunk* prev;
	Chunk* next;
};

public:

	FSAllocator(int elements) : m_MaxElements(elements) , m_FirstUsed(0) , m_Size(0) {
		char *pMem = new char[ m_MaxElements * sizeof(Chunk) ];
		m_Memory = (Chunk*)pMem; 

		// Set the free list first pointer
		m_FirstFree = m_Memory;

		// Clear the memory
		memset( m_Memory, 0, sizeof( Chunk) * m_MaxElements );

		// Point at first element
		Chunk* pElement = m_FirstFree;

		// Set the double linked free list
		for( int i = 0; i < m_MaxElements; ++i ) {
			pElement->prev = pElement - 1;
			pElement->next = pElement + 1;
			++pElement;
		}

		// first element should have a null prev
		m_FirstFree->prev = NULL;
		// last element should have a null next
		(pElement-1)->next = NULL;
	}

	~FSAllocator(void) {
		 delete [](char *)m_Memory;
	}

	// Allocate a new Type and return a pointer to it 
	Type *alloc() {
		Chunk *pNewNode = NULL;

		if( !m_FirstFree ) {
			return NULL;
		}
		else {
			pNewNode = m_FirstFree;
			m_FirstFree = pNewNode->next;

			// if the new node points to another free node then
			// change that nodes prev free pointer...
			if( pNewNode->next ) {
				pNewNode->next->prev = NULL;
			}

			// node is now on the used list

			pNewNode->prev = NULL; // the allocated node is always first in the list

			if( m_FirstUsed == NULL ) {
				pNewNode->next = NULL; // no other nodes
			}
			else {
				m_FirstUsed->prev = pNewNode; // insert this at the head of the used list
				pNewNode->next = m_FirstUsed;
			}

			m_FirstUsed = pNewNode;
			++m_Size;
		}       
		return reinterpret_cast<Type*>(pNewNode);
	}

	void free( Type* user_data ) {
		Chunk* pNode = reinterpret_cast<Chunk*>(user_data);

		// manage used list, remove this node from it
		if( pNode->prev ) {
			pNode->prev->next = pNode->next;
		}
		else {
			// this handles the case that we delete the first node in the used list
			m_FirstUsed = pNode->next;
		}

		if( pNode->next ) {
			pNode->next->prev = pNode->prev;
		}

		// add to free list
		if( m_FirstFree == NULL ) {
			// free list was empty
			m_FirstFree = pNode;
			pNode->prev = NULL;
			pNode->next = NULL;
		}
		else {
			// Add this node at the start of the free list
			m_FirstFree->prev = pNode;
			pNode->next = m_FirstFree;
			m_FirstFree = pNode;
		}
		--m_Size;
	}

	const int size() const {
		return m_Size;
	}
private:
	int m_MaxElements;
	Chunk* m_Memory;
	Chunk *m_FirstFree;
	Chunk *m_FirstUsed;
	int m_Size;
};

}
