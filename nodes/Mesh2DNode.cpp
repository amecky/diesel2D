#include "Mesh2DNode.h"
#include "..\utils\Log.h"

namespace ds {

const uint32 VERTEX_SIZE = 40;
const uint32 INDEX_SIZE = 4;
// ------------------------------------------------------------
//
// ------------------------------------------------------------
Mesh2D::Mesh2D(uint32 maxVertices,uint32 maxIndices) : m_MaxVertices(maxVertices) , m_MaxIndices(maxIndices) , m_Active(true) {
	m_VertexCounter = 0;
	m_IndexCounter = 0;
	m_TriangleCounter = 0;
	m_BufferSize = VERTEX_SIZE * maxVertices + INDEX_SIZE * maxIndices;
	m_StartIndices = VERTEX_SIZE * maxVertices;
	m_Buffer = new char[m_BufferSize];
	//gEngine->getDrawCounter()->addMemory(m_BufferSize);
	LOGC(logINFO,"Mesh2D") << "creating new mesh - buffer size " << m_BufferSize << " start index buffer " << m_StartIndices;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
Mesh2D::~Mesh2D() {
	//gEngine->getDrawCounter()->removeMemory(m_BufferSize);
	delete[] m_Buffer;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void Mesh2D::setUV(uint32 index,float u,float v) {
	assert(index < m_MaxVertices);
	assert(VERTEX_SIZE*index < m_StartIndices);
	char* buffer = m_Buffer + VERTEX_SIZE * index;
	(*(TTCVertex*)buffer).tu = u;
	(*(TTCVertex*)buffer).tv = v;
}

// -------------------------------------------------------
// Set uv coordinates based on texture atlas item
// -------------------------------------------------------
void Mesh2D::setUV(uint32 index,int offset,const char* taItem) {
	float u1,v1,u2,v2;
	//TextureAtlas* at = m_Material->textureAtlas;
	//assert(at != 0);
	//at->getTextureCoordinates(taItem,&u1,&v1,&u2,&v2);
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void Mesh2D::setVertex(uint32 index,float x,float y) {
	assert(index < m_MaxVertices);	
	assert(VERTEX_SIZE*index < m_StartIndices);
	char* buffer = m_Buffer + VERTEX_SIZE * index;
	(*(TTCVertex*)buffer).x = x;
	(*(TTCVertex*)buffer).y = y;
	(*(TTCVertex*)buffer).z = 0.0f;
	(*(TTCVertex*)buffer).rwh = 1.0f;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void Mesh2D::setColor(uint32 index,const Color& color) {
	assert(index < m_MaxVertices);
	assert(VERTEX_SIZE*index < m_StartIndices);
	char* buffer = m_Buffer + VERTEX_SIZE * index;
	(*(TTCVertex*)buffer).color = color;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
void Mesh2D::setIndex(uint32 index,uint32 idx) {
	assert(index < m_MaxIndices);
	assert((m_StartIndices + INDEX_SIZE * index) < m_BufferSize);
	char* buffer = m_Buffer + INDEX_SIZE * index + m_StartIndices;
	(*(uint32*)buffer) = idx;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
uint32 Mesh2D::fillBuffer(TTCBuffer* buffer,uint32 vertexStart) {
	if ( m_VertexCounter > 0 ) {
		char* vBuffer = m_Buffer;
		for ( uint32 i = 0; i < m_VertexCounter;++i ) {
			buffer->add((*(TTCVertex*)vBuffer));
			vBuffer += VERTEX_SIZE;
		}		
		char* iBuffer = m_Buffer + m_StartIndices;
		for ( uint32 i = 0; i < m_IndexCounter; ++i ) {
			buffer->addIndex((*(uint32*)iBuffer)+vertexStart);
			iBuffer += INDEX_SIZE;
		}
	}
	return m_TriangleCounter;
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
Mesh2DNode::Mesh2DNode(const char* name,Renderer* renderer,int material,uint32 maxVertices,uint32 maxIndices) 
	: Node(name,renderer,material) , m_MaxVertices(maxVertices) , m_MaxIndices(maxIndices) , m_VertexCounter(0) {
	
	m_Buffer = new TTCBuffer(renderer,m_MaxVertices,m_MaxIndices,true);	
}


Mesh2DNode::~Mesh2DNode() {
	LOGC(logINFO,"Mesh2DNode") << "destructing Mesh2DNode " << getNodeName();
	clear();
}

void Mesh2DNode::clear() {
	Meshes::iterator it = m_Meshes.begin();
	while ( it != m_Meshes.end() ) {
		delete (*it);
		it = m_Meshes.erase(it);
	}
}

Mesh2D* Mesh2DNode::create(uint32 maxVertices,uint32 maxIndices) {
	Mesh2D* m = new Mesh2D(maxVertices,maxIndices);
	m_Meshes.push_back(m);
	return m;
}

void Mesh2DNode::add(Mesh2D* mesh) {
	m_Meshes.push_back(mesh);
}

void Mesh2DNode::update(float elapsed) {
	for ( size_t i = 0; i < m_Meshes.size();++i ) {
		m_Meshes[i]->update(elapsed);
	}
}

void Mesh2DNode::prepareRendering() {
	m_VertexCounter = 0;
	uint32 counter = 0;
	for ( size_t i = 0; i < m_Meshes.size();++i ) {
		Mesh2D* m = m_Meshes[i];
		if ( m->isActive() ) {
			m_VertexCounter += m->fillBuffer(m_Buffer,counter);
			counter += m->getVertexCounter();
		}
	}
	m_Buffer->prepareData();
}

void Mesh2DNode::draw() {
	m_Buffer->render(m_VertexCounter);	
}

void Mesh2DNode::postRendering() {
	m_Buffer->clear();
}

void Mesh2DNode::debug() {
	LOGC(logINFO,"World") << "   Mesh2DNode -> Meshes: " << m_Meshes.size() << " Vertices: " << m_VertexCounter;
}


}
