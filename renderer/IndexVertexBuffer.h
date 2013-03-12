#pragma once
#include "..\dxstdafx.h"
#include "VertexDeclaration.h"
#include "Renderer.h"
#include "render_types.h"
#include "vertex_types.h"
#include "..\utils\Log.h"

namespace ds {
// --------------------------------------------------------
// This class is a template based vertex buffer. It can
// handle any kind of vertex type.
// --------------------------------------------------------
template <class T>
class IndexVertexBuffer {

public:
	//IndexVertexBuffer(D3DPRIMITIVETYPE primitiveType,int vertexDefintion,int maxVertices,bool dynamic = false);
	IndexVertexBuffer(Renderer* renderer,D3DPRIMITIVETYPE primitiveType,int vertexDefinition,int maxVertices,bool dynamic = false);
	~IndexVertexBuffer(void);
	void add(const T& v);
	void clear();
	void update(DWORD flags = D3DLOCK_DISCARD);
	void prepareData();
	void render(int numPrimitives);
	void createIndexBuffer(int maxIndices);
	void addIndices(int indexArray[],int count);
	void addIndex(int idx);
	void debug();
private:
	LPDIRECT3DVERTEXBUFFER9 m_VB;
	T* m_Data;
	int m_Counter;
	int m_MaxVertices;
	VertexDeclaration* m_VertexDeclaration;	
    int m_VertexSize;
	D3DPRIMITIVETYPE m_PrimitiveType;
	bool m_Dynamic;
	// the index buffer	
	int m_MaxIndices;
	WORD* m_Indices;
	LPDIRECT3DINDEXBUFFER9 m_IndexBuffer;
	//int m_IBHandle;
	int m_IndexCounter;
	Renderer* m_Renderer;
};

// --------------------------------------------------------
// Constructor to create a new vertex buffer. The buffer
// will be created as dynamic using the maxVertices
// size and also an internal data array is created.
//
// [in] primitiveType - the type of primitive to use
// [in] vertexDeclaration - the vertex declaration of the vertex
// [in] vertexSize - sizeof vertex
// [in] maxVertices - the max number of vertices 
// --------------------------------------------------------
/*
template<class T>
IndexVertexBuffer<T>::IndexVertexBuffer(D3DPRIMITIVETYPE primitiveType,int vertexDefintion,int maxVertices,bool dynamic) 
	: m_PrimitiveType(primitiveType) , m_MaxVertices(maxVertices) , m_Dynamic(dynamic) {
	m_Data = new T[maxVertices];
	VDStruct vd = m_Renderer->getVertexDeclaration(vertexDefintion);
	m_VertexDeclaration = vd.declaration;
	m_VertexSize = vd.vertexSize;
	IDirect3DDevice9 * pDevice = gEngine->getDevice();
	D3DPOOL pool = m_Dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
	DWORD usage = m_Dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
	LOGC(logINFO,"IVBuffer") << "creating new vertex buffer - size: " << m_MaxVertices << " dynamic: " << m_Dynamic;
	HR(pDevice->CreateVertexBuffer( m_MaxVertices * m_VertexSize,usage,0 ,pool, &m_VB, NULL ))
	m_Counter = 0;
	m_IndexBuffer = 0;
	m_IndexCounter = 0;
}
*/
template<class T>
IndexVertexBuffer<T>::IndexVertexBuffer(Renderer* renderer,D3DPRIMITIVETYPE primitiveType,int vertexDefinition,int maxVertices,bool dynamic) 
	: m_Renderer(renderer) , m_PrimitiveType(primitiveType) , m_MaxVertices(maxVertices) , m_Dynamic(dynamic) {
	VDStruct vd = m_Renderer->getVertexDeclaration(vertexDefinition);
	m_VertexDeclaration = vd.declaration;
	m_VertexSize = vd.vertexSize;
	m_Data = new T[maxVertices];
	//m_IBHandle = -1;
	D3DPOOL pool = m_Dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
	DWORD usage = m_Dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
	LOGC(logINFO,"IVBuffer") << "creating new vertex buffer - size: " << m_MaxVertices << " vertexDefinition: " << vertexDefinition << " vertexSize: " << m_VertexSize << " dynamic: " << (m_Dynamic == 0 ? false : true);
	HR(renderer->getDevice()->CreateVertexBuffer( m_MaxVertices * m_VertexSize,usage,0 ,pool, &m_VB, NULL ))
	m_Counter = 0;
	m_IndexBuffer = 0;
	m_IndexCounter = 0;
}

template<class T>
IndexVertexBuffer<T>::~IndexVertexBuffer(void) {
	LOGC(logINFO,"IVBuffer") << "destructing buffer";
	if( m_VB != 0 ) { 
		m_VB->Release(); 
		m_VB = 0; 
	} 
	delete[] m_Data;
	if ( m_IndexBuffer != 0 ) {
		m_IndexBuffer->Release();
		delete[] m_Indices;
	}
}

// --------------------------------------------------------
// This method adds a vertex to the internal data array.
// It will also check that the number of vertices does not
// exceed the define limit of maxVertices
//
// [in] v - the vertex
// --------------------------------------------------------
template<class T>
void IndexVertexBuffer<T>::add(const T& v) {
	if ( m_Counter < m_MaxVertices ) {
		m_Data[m_Counter] = v;
		++m_Counter;	
	}
}

// --------------------------------------------------------
// This method will reset the internal counter and therefore
// it will clear the internal data array
// --------------------------------------------------------
template <class T>
void IndexVertexBuffer<T>::clear() {
	m_Counter = 0;
	m_IndexCounter = 0;
}

// --------------------------------------------------------
// This method will copy the internal data to the vertex
// buffer memory 
//
// [in] flags - flags to use for the Lock - default D3DLOCK_DISCARD
// --------------------------------------------------------
template <class T>
void IndexVertexBuffer<T>::update(DWORD flags) {
	if ( m_Counter > 0 ) {
		DWORD flag = 0;
		if ( m_Dynamic ) {
			flag = D3DLOCK_DISCARD;
		}
		void *pData; 
		// Lock the vertex buffer 
		HR(m_VB->Lock( 0, 0, (void**)&pData, flag ));
		// Copy vertices to vertex buffer 
		memcpy( pData, m_Data, m_Counter * m_VertexSize );
		HR(m_VB->Unlock());
		/*
		if ( m_IBHandle != -1 ) {
			WORD* pIndex;
			m_Renderer->lockBuffer(m_IBHandle,0,m_IndexCounter,0,( void** )&pIndex);
			memcpy( pIndex, m_Indices, m_IndexCounter * sizeof(WORD) );
			m_Renderer->unlockBuffer(m_IBHandle);
		}
		*/
		if ( m_IndexBuffer != 0 ) {
			WORD *pIndex;
			HR(m_IndexBuffer->Lock( 0, sizeof(WORD), ( void** )&pIndex, flag ));					
			memcpy( pIndex, m_Indices, m_IndexCounter * sizeof(WORD) );
			HR(m_IndexBuffer->Unlock());
		}
	}
}

// --------------------------------------------------------
// This method is used internal to prepare the rendering
// --------------------------------------------------------
template<class T> 
void IndexVertexBuffer<T>::prepareData() {
	if ( m_Counter > 0 ) {		
		if ( m_Dynamic ) {
			update();				
		}
		m_Renderer->getDevice()->SetStreamSource( 0, m_VB, 0, m_VertexSize ); 
		m_Renderer->getDevice()->SetVertexDeclaration(m_VertexDeclaration->get());
	}
}

// --------------------------------------------------------
// This method is used internal to render the vertex 
// buffer. It requires the number of primitives.
//
// [in] numPrimitives - the number of primitives
// --------------------------------------------------------
template<class T> 
void IndexVertexBuffer<T>::render(int numPrimitives) {	
	if ( m_Counter > 0 ) {
		if ( m_IndexBuffer != 0 ) {
			HR(m_Renderer->getDevice()->SetIndices( m_IndexBuffer)); 
			//int start = m_Renderer->drawBuffer(m_IBHandle);
			HR(m_Renderer->getDevice()->DrawIndexedPrimitive( m_PrimitiveType, 0, 0, m_Counter, 0, numPrimitives ));
			m_Renderer->getDrawCounter().addIndices(m_IndexCounter);
			m_Renderer->getDrawCounter().addPrimitives(m_Counter);
			m_Renderer->getDrawCounter().addDrawCall();
		} 
		else {
			HR(m_Renderer->getDevice()->DrawPrimitive(m_PrimitiveType,0,numPrimitives));
			m_Renderer->getDrawCounter().addPrimitives(m_Counter);
			m_Renderer->getDrawCounter().addDrawCall();
		}
	}
}

// --------------------------------------------------------
// This method will create an index buffer to be used
// with the vertex buffer
//
// [in] maxIndices - max number of indices
// --------------------------------------------------------
template<class T> 
void IndexVertexBuffer<T>::createIndexBuffer(int maxIndices) {
	/*
	m_IBHandle = m_Renderer->createBufferHandle(PT_IDX,maxIndices,GBT_INDEX);
	m_MaxIndices = maxIndices;
	m_Indices = new WORD[m_MaxIndices];
	LOGC(logINFO,"IVBuffer") << "new IndexBuffer created size: " << maxIndices << " handle: " << m_IBHandle << " dynamic: " << (m_Dynamic == 0 ? false : true);
	*/
	m_MaxIndices = maxIndices;
	m_Indices = new WORD[m_MaxIndices];
	D3DPOOL pool = m_Dynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED; 
	DWORD usage = m_Dynamic ? D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC : D3DUSAGE_WRITEONLY; 			
	HR(m_Renderer->getDevice()->CreateIndexBuffer( m_MaxIndices * sizeof(WORD),usage,D3DFMT_INDEX16,pool,&m_IndexBuffer,NULL));	
	LOGC(logINFO,"IVBuffer") << "new IndexBuffer created size: " << maxIndices << (m_Dynamic == 0 ? false : true);
}

// --------------------------------------------------------
// Adds an array to the internal data array
//
// [in] indexArray - array of indices to add
// [in] count - the size of this array
// --------------------------------------------------------
template<class T> 
void IndexVertexBuffer<T>::addIndices(int indexArray[],int count) {
	if ( (m_IndexCounter+count) < m_MaxIndices ) {
		for ( int i = 0; i < count; ++i ) {
			m_Indices[m_IndexCounter] = indexArray[i];
			++m_IndexCounter;
		}
	}
}

// --------------------------------------------------------
// Adds one index to the internal array
//
// [in] idx - the index to add
// --------------------------------------------------------
template<class T> 
void IndexVertexBuffer<T>::addIndex(int idx) {
	if ( m_IndexCounter < m_MaxIndices ) {
		m_Indices[m_IndexCounter] = idx;
		++m_IndexCounter;
	}
}

template<class T> 
void IndexVertexBuffer<T>::debug() {
	LOGC(logINFO,"World") << "   IndexVertexBuffer vertices: " << m_Counter << " indices: " << m_IndexCounter;
}
// --------------------------------------------------------
// Ready to use Buffers
// --------------------------------------------------------

// --------------------------------------------------------
// Transformed position and textured
// --------------------------------------------------------
/*
class TransformedTexturedBuffer : public IndexVertexBuffer<TransformedTexturedVertex> {

public:
	TransformedTexturedBuffer(int vertexSize,int indexSize,bool dynamic) :
		IndexVertexBuffer<TransformedTexturedVertex>(D3DPT_TRIANGLELIST,TransformedTexturedVertex::getVertexDeclaration(),sizeof(TransformedTexturedVertex),vertexSize,dynamic) {
		createIndexBuffer(indexSize);
	}

};
*/
// -------------------------------------------------------
// Transformed textures colors
// -------------------------------------------------------
class TTCBuffer : public IndexVertexBuffer<TTCVertex> {

public:
	TTCBuffer(Renderer* renderer,int vertexSize,int indexSize,bool dynamic) :
	  IndexVertexBuffer<TTCVertex>(renderer,D3DPT_TRIANGLELIST,VD_TTC,vertexSize,dynamic) {
		  createIndexBuffer(indexSize);
	  }

};

// --------------------------------------------------------
// Transformed position and coloured
// --------------------------------------------------------
/*
class TransformedColorVertexBuffer : public IndexVertexBuffer<TransformedColorVertex> {

public:
	TransformedColorVertexBuffer(int vertexSize,int indexSize,bool dynamic) :
	  IndexVertexBuffer<TransformedColorVertex>(D3DPT_TRIANGLELIST,TransformedColorVertex::getVertexDeclaration(),sizeof(TransformedColorVertex),vertexSize,dynamic) {
		  createIndexBuffer(indexSize);
	  }

};
*/
// --------------------------------------------------------
// Transformed position and textured
// --------------------------------------------------------
/*
class TexturedNormalsBuffer : public IndexVertexBuffer<PositionTextureNormalsVertex> {

public:
	TexturedNormalsBuffer(int vertexSize,int indexSize,bool dynamic) :
	  IndexVertexBuffer<PositionTextureNormalsVertex>(D3DPT_TRIANGLELIST,PositionTextureNormalsVertex::getVertexDeclaration(),sizeof(PositionTextureNormalsVertex),vertexSize,dynamic) {
		  createIndexBuffer(indexSize);
	  }

};
*/
// --------------------------------------------------------
// Position, texture coords, normals, color
// --------------------------------------------------------
/*
class PTNCBuffer : public IndexVertexBuffer<PTNCVertex> {

public:
	PTNCBuffer(int vertexSize,int indexSize,bool dynamic) :
	  IndexVertexBuffer<PTNCVertex>(D3DPT_TRIANGLELIST,PTNCVertex::getVertexDeclaration(),sizeof(PTNCVertex),vertexSize,dynamic) {
		  createIndexBuffer(indexSize);
	  }

};
*/
}