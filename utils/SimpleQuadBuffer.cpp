#include "SimpleQuadBuffer.h"

namespace ds {

const uint32 QUAD_SIZE = 188;
const uint32 INDEX_SIZE = 4;
// ------------------------------------------------------------
//
// ------------------------------------------------------------
SimpleQuadBuffer::SimpleQuadBuffer(Renderer* renderer,uint32 maxQuads) : m_VertexCounter(0) , m_Index(0) {
	m_MaxVertices = 4 * maxQuads;
	m_MaxIndices = 6 * maxQuads;
	m_BufferSize = QUAD_SIZE * m_MaxVertices + INDEX_SIZE * m_MaxIndices;
	m_StartIndices = QUAD_SIZE * m_MaxVertices;
	m_DataBuffer = new char[m_BufferSize];
	//gEngine->getDrawCounter()->addMemory(m_BufferSize);
	LOGC(logINFO,"SimpleQuadBuffer") << "creating new SimpleQuadBuffer - buffer size " << m_BufferSize << " start index buffer " << m_StartIndices;
	// fill up all indices
	char* indexBuffer = m_DataBuffer + m_StartIndices;
	for ( uint32 i = 0; i < maxQuads;++i ) {
		(*(uint32*)indexBuffer) = i * 4 + 0;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 1;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 2;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 2;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 3;
		indexBuffer += INDEX_SIZE;
		(*(uint32*)indexBuffer) = i * 4 + 0;
		indexBuffer += INDEX_SIZE;
	}	
	m_Buffer = new TTCBuffer(renderer,m_MaxVertices,m_MaxIndices,true);	
}


SimpleQuadBuffer::~SimpleQuadBuffer() {
	LOGC(logINFO,"SimpleQuadBuffer") << "destructing SimpleQuadBuffer";
	//m_Ren->getDrawCounter()->removeMemory(m_BufferSize);
	delete m_Buffer;
	delete m_DataBuffer;
}

void SimpleQuadBuffer::init() {	
	
}

uint32 SimpleQuadBuffer::create(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color) {
	uint32 idx = m_Index;
	if ( !m_FreeList.empty()) {
		idx = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else {
		++m_Index;
	}
	float u1,v1,u2,v2;
	ds::math::getTextureCoordinates(rect,textureSize,&u1,&v1,&u2,&v2);
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).scaleX = 1.0f;	
	(*(Plane*)buffer).scaleY = 1.0f;
	(*(Plane*)buffer).dimX = dimX;
	(*(Plane*)buffer).dimY = dimY;
	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	(*(Plane*)buffer).v1.x = 0.0f;
	(*(Plane*)buffer).v1.y = 0.0f;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v1.color = color;

	(*(Plane*)buffer).v2.x = dimX;
	(*(Plane*)buffer).v2.y = 0.0f;
	(*(Plane*)buffer).v2.z = 0.0f;
	(*(Plane*)buffer).v2.rwh = 1.0f;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v2.color = color;

	(*(Plane*)buffer).v3.x = dimX;
	(*(Plane*)buffer).v3.y = dimY;
	(*(Plane*)buffer).v3.z = 0.0f;
	(*(Plane*)buffer).v3.rwh = 1.0f;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v3.color = color;

	(*(Plane*)buffer).v4.x = 0.0f;
	(*(Plane*)buffer).v4.y = dimY;
	(*(Plane*)buffer).v4.z = 0.0f;
	(*(Plane*)buffer).v4.rwh = 1.0f;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
	(*(Plane*)buffer).v4.color = color;

	m_VertexCounter += 4;
	return idx;
}
// -------------------------------------------------------
// Set texture coordinates directly
// -------------------------------------------------------
void SimpleQuadBuffer::setTextureCoordinates(uint32 idx,float u1,float v1,float u2,float v2) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;	
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
}

void SimpleQuadBuffer::update(uint32 index,const Vec2& pos) {
	char* buffer = m_DataBuffer + index * QUAD_SIZE;
	float dimX = (*(Plane*)buffer).scaleX * (*(Plane*)buffer).dimX;	
	float dimY = (*(Plane*)buffer).scaleY * (*(Plane*)buffer).dimY;	
	

	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	(*(Plane*)buffer).v1.x = pos.x;
	(*(Plane*)buffer).v1.y = pos.y;

	(*(Plane*)buffer).v2.x = pos.x + dimX;
	(*(Plane*)buffer).v2.y = pos.y;

	(*(Plane*)buffer).v3.x = pos.x + dimX;
	(*(Plane*)buffer).v3.y = pos.y + dimY;

	(*(Plane*)buffer).v4.x = pos.x;
	(*(Plane*)buffer).v4.y = pos.y + dimY;
}

void SimpleQuadBuffer::getPosition(uint32 idx,Vec2* outVec) {
	char* buffer = m_DataBuffer + idx * QUAD_SIZE;
	outVec->x = (*(Plane*)buffer).x;
	outVec->y = (*(Plane*)buffer).y;
}

void SimpleQuadBuffer::setPosition(uint32 idx,const Vec2& v) {
	update(idx,v);
}

void SimpleQuadBuffer::remove(uint32 index) {
	char* buffer = m_DataBuffer + QUAD_SIZE * index;
	(*(Plane*)buffer).v1.rwh = 0.0f;
	(*(Plane*)buffer).v2.rwh = 0.0f;		
	(*(Plane*)buffer).v3.rwh = 0.0f;
	(*(Plane*)buffer).v4.rwh = 0.0f;
	m_VertexCounter -= 4;
	m_FreeList.push_back(index);
}


// -------------------------------------------------------
// get specific vertex position
// -------------------------------------------------------
void SimpleQuadBuffer::getPosition(uint32 idx,uint32 offset,Vec2* outVec) {
	assert(offset>= 0);
	assert(offset<4);
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	if ( offset == 0 ) {
		outVec->x = (*(Plane*)buffer).v1.x;
		outVec->y = (*(Plane*)buffer).v1.y;
	}
	else if ( offset == 1 ) {
		outVec->x = (*(Plane*)buffer).v2.x;
		outVec->y = (*(Plane*)buffer).v2.y;
	}
	else if ( offset == 2 ) {
		outVec->x = (*(Plane*)buffer).v3.x;
		outVec->y = (*(Plane*)buffer).v3.y;
	}
	else if ( offset == 3 ) {
		outVec->x = (*(Plane*)buffer).v4.x;
		outVec->y = (*(Plane*)buffer).v4.y;
	}
}

// -------------------------------------------------------
// set specific vertex position
// -------------------------------------------------------
void SimpleQuadBuffer::setPosition(uint32 idx,uint32 offset,const Vec2& v) {
	assert(offset>= 0);
	assert(offset<4);
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	if ( offset == 0 ) {
		(*(Plane*)buffer).v1.x = v.x;
		(*(Plane*)buffer).v1.y = v.y;
	}	
	else if ( offset == 1 ) {
		(*(Plane*)buffer).v2.x = v.x;
		(*(Plane*)buffer).v2.y = v.y;
	}	
	else if ( offset == 2 ) {
		(*(Plane*)buffer).v3.x = v.x;
		(*(Plane*)buffer).v3.y = v.y;
	}	
	else if ( offset == 3 ) {
		(*(Plane*)buffer).v4.x = v.x;
		(*(Plane*)buffer).v4.y = v.y;
	}	
}
// --------------------------------------------------
// Set texture rect
// --------------------------------------------------
void SimpleQuadBuffer::setTextureRect(uint32 idx,const Rect& textureRect,float textureSize) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	float u1,v1,u2,v2;
	ds::math::getTextureCoordinates(textureRect,textureSize,&u1,&v1,&u2,&v2);
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
}

// --------------------------------------------------
// Set color
// --------------------------------------------------
void SimpleQuadBuffer::setColor(uint32 idx,const Color& color) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).v1.color = color;
	(*(Plane*)buffer).v2.color = color;
	(*(Plane*)buffer).v3.color = color;
	(*(Plane*)buffer).v4.color = color;
}

void SimpleQuadBuffer::prepareRendering() {
	if ( m_VertexCounter > 0 ) {
		char* vBuffer = m_DataBuffer;
		for ( uint32 i = 0; i < m_Index;++i ) {
			float rwh = (*(Plane*)vBuffer).v1.rwh;
			if ( rwh != 0.0f ) {
				m_Buffer->add((*(Plane*)vBuffer).v1);
				m_Buffer->add((*(Plane*)vBuffer).v2);
				m_Buffer->add((*(Plane*)vBuffer).v3);
				m_Buffer->add((*(Plane*)vBuffer).v4);
			}
			vBuffer += QUAD_SIZE;
		}		
		uint32 indexCounter = m_VertexCounter / 4 * 6;
		char* iBuffer = m_DataBuffer + m_StartIndices;
		for ( uint32 i = 0; i < indexCounter; ++i ) {
			m_Buffer->addIndex((*(uint32*)iBuffer));
			iBuffer += INDEX_SIZE;
		}
	}
	m_Buffer->prepareData();
}

void SimpleQuadBuffer::draw() {
	m_Buffer->render(m_VertexCounter/2);	
}

void SimpleQuadBuffer::postRendering() {
	m_Buffer->clear();
}

void SimpleQuadBuffer::debug() {
	//LOGC(logINFO,"World") << "   Mesh2DNode -> Meshes: " << m_Meshes.size() << " Vertices: " << m_VertexCounter;
}


}
