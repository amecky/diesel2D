#include "SimpleQuadNode.h"
#include "..\content\TextureAtlas.h"
#include "..\utils\Profiler.h"
#include "..\renderer\vertex_types.h"
#include "..\math\GameMath.h"

namespace ds {

const uint32 QUAD_SIZE = 188;
const uint32 INDEX_SIZE = 4;
// ------------------------------------------------------------
// Simple quad node
// ------------------------------------------------------------
SimpleQuadNode::SimpleQuadNode(const char* nodeName,Renderer* renderer,int material,uint32 maxQuads) : Node(nodeName,renderer,material) , m_VertexCounter(0) , m_Index(0) {
	m_MaxVertices = 4 * maxQuads;
	m_MaxIndices = 6 * maxQuads;
	m_BufferSize = QUAD_SIZE * m_MaxVertices + INDEX_SIZE * m_MaxIndices;
	m_StartIndices = QUAD_SIZE * m_MaxVertices;
	m_DataBuffer = new char[m_BufferSize];
	renderer->getDrawCounter().addMemory(m_BufferSize);
	LOGC(logINFO,"SimpleQuadNode") << "creating new SimpleQuadNode - buffer size " << m_BufferSize << " start index buffer at " << m_StartIndices;
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
	//m_Buffer = new TTCBuffer(renderer,m_MaxVertices,m_MaxIndices,true);	
	m_Handle = renderer->createBufferHandle(PT_TRI,VD_TTC,GBT_BOTH,true);
}

// -------------------------------------------------------
// Destructor
// -------------------------------------------------------
SimpleQuadNode::~SimpleQuadNode() {
	LOGC(logINFO,"SimpleQuadNode") << "destructing SimpleQuadNode";
	//gEngine->getDrawCounter()->removeMemory(m_BufferSize);
	//delete m_Buffer;
	delete m_DataBuffer;
}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void SimpleQuadNode::init() {	
	//LOGC(logINFO,"SimpleQuadNode") << getName();
	
}

// -------------------------------------------------------
// Create with texture rect
// -------------------------------------------------------
uint32 SimpleQuadNode::create(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color) {
	float u1,v1,u2,v2;
	ds::math::getTextureCoordinates(rect,textureSize,&u1,&v1,&u2,&v2);
	return create(dimX,dimY,pos,u1,v1,u2,v2,color);	
}

// -------------------------------------------------------
// Create with texture atlas item
// -------------------------------------------------------
uint32 SimpleQuadNode::create(float dimX,float dimY,const Vec2& pos,const char* taItem,const Color& color) {
	float u1,v1,u2,v2;		
	m_Renderer->getTextureCoordinates(m_Material,taItem,&u1,&v1,&u2,&v2);
	return create(dimX,dimY,pos,u1,v1,u2,v2,color);	
}

// -----------------------------------------------------------
// Create with texture atlas item and size from texture rect
// -----------------------------------------------------------
uint32 SimpleQuadNode::create(const Vec2& pos,const char* taItem,const Color& color) {
	float u1,v1,u2,v2;
	m_Renderer->getTextureCoordinates(m_Material,taItem,&u1,&v1,&u2,&v2);
	float dimX = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	float dimY = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	return create(dimX,dimY,pos,u1,v1,u2,v2,color);	
}
// -------------------------------------------------------
// Create with direct texture coordinates
// -------------------------------------------------------
uint32 SimpleQuadNode::create(float dimX,float dimY,const Vec2& pos,float u1,float v1,float u2,float v2,const Color& color) {
	uint32 idx = m_Index;
	if ( !m_FreeList.empty()) {
		idx = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else {
		++m_Index;
	}		
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).scaleX = 1.0f;	
	(*(Plane*)buffer).scaleY = 1.0f;
	(*(Plane*)buffer).dimX = dimX;
	(*(Plane*)buffer).dimY = dimY;
	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	(*(Plane*)buffer).v1.x = pos.x;
	(*(Plane*)buffer).v1.y = pos.y;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v1.color = color;

	(*(Plane*)buffer).v2.x = pos.x + dimX;
	(*(Plane*)buffer).v2.y = pos.y;
	(*(Plane*)buffer).v2.z = 0.0f;
	(*(Plane*)buffer).v2.rwh = 1.0f;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v2.color = color;

	(*(Plane*)buffer).v3.x = pos.x + dimX;
	(*(Plane*)buffer).v3.y = pos.y + dimY;
	(*(Plane*)buffer).v3.z = 0.0f;
	(*(Plane*)buffer).v3.rwh = 1.0f;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v3.color = color;

	(*(Plane*)buffer).v4.x = pos.x;
	(*(Plane*)buffer).v4.y = pos.y + dimY;
	(*(Plane*)buffer).v4.z = 0.0f;
	(*(Plane*)buffer).v4.rwh = 1.0f;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
	(*(Plane*)buffer).v4.color = color;

	m_VertexCounter += 4;
	return idx;
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void SimpleQuadNode::update(uint32 index,float scaleX,float scaleY,const Vec2& pos) {
	char* buffer = m_DataBuffer + index * QUAD_SIZE;
	(*(Plane*)buffer).scaleX = scaleX;	
	(*(Plane*)buffer).scaleY = scaleY;
	update(index,pos);	
}

// -------------------------------------------------------
// Update by position only
// -------------------------------------------------------
void SimpleQuadNode::update(uint32 index,const Vec2& pos) {
	char* buffer = m_DataBuffer + index * QUAD_SIZE;
	float scaleX = (*(Plane*)buffer).scaleX;	
	float scaleY = (*(Plane*)buffer).scaleY;
	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	float dx = (*(Plane*)buffer).dimX * scaleX;
	float dy = (*(Plane*)buffer).dimY * scaleY;

	(*(Plane*)buffer).v1.x = pos.x;
	(*(Plane*)buffer).v1.y = pos.y;

	(*(Plane*)buffer).v2.x = pos.x + dx;
	(*(Plane*)buffer).v2.y = pos.y;

	(*(Plane*)buffer).v3.x = pos.x + dx;
	(*(Plane*)buffer).v3.y = pos.y + dy;

	(*(Plane*)buffer).v4.x = pos.x;
	(*(Plane*)buffer).v4.y = pos.y + dy;
}

// -------------------------------------------------------
// Get position of quad
// -------------------------------------------------------
void SimpleQuadNode::getPosition(uint32 idx,Vec2* outVec) {
	char* buffer = m_DataBuffer + idx * QUAD_SIZE;
	outVec->x = (*(Plane*)buffer).x;
	outVec->y = (*(Plane*)buffer).y;
}

// -------------------------------------------------------
// Set quad position
// -------------------------------------------------------
void SimpleQuadNode::setPosition(uint32 idx,const Vec2& v) {
	update(idx,v);
}

// -------------------------------------------------------
// Remove quad
// -------------------------------------------------------
void SimpleQuadNode::remove(uint32 index) {
	char* buffer = m_DataBuffer + QUAD_SIZE * index;	
	(*(Plane*)buffer).v1.rwh = 0.0f;
	(*(Plane*)buffer).v2.rwh = 0.0f;		
	(*(Plane*)buffer).v3.rwh = 0.0f;
	(*(Plane*)buffer).v4.rwh = 0.0f;
	m_VertexCounter -= 4;
	//LOG(logINFO) << "removing " << index << " vertext counter " << m_VertexCounter;
	m_FreeList.push_back(index);
}

// -------------------------------------------------------
// Get position of single vertex
// -------------------------------------------------------
void SimpleQuadNode::getPosition(uint32 idx,uint32 offset,Vec2* outVec) {
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
// Set position of single vertex
// -------------------------------------------------------
void SimpleQuadNode::setPosition(uint32 idx,uint32 offset,const Vec2& v) {
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
void SimpleQuadNode::setTextureRect(uint32 idx,const Rect& textureRect,float textureSize) {
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
// Set texture rect with TextureAtlas item
// --------------------------------------------------
void SimpleQuadNode::setTextureRect(uint32 idx,const char* taItem) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	float u1,v1,u2,v2;		
	m_Renderer->getTextureCoordinates(m_Material,taItem,&u1,&v1,&u2,&v2);
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
}
// -------------------------------------------------------
// Set texture coordinates directly
// -------------------------------------------------------
void SimpleQuadNode::setTextureCoordinates(uint32 idx,float u1,float v1,float u2,float v2) {
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
// --------------------------------------------------
// Set color
// --------------------------------------------------
void SimpleQuadNode::setColor(uint32 idx,const Color& color) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).v1.color = color;
	(*(Plane*)buffer).v2.color = color;
	(*(Plane*)buffer).v3.color = color;
	(*(Plane*)buffer).v4.color = color;
}

// -------------------------------------------------------
// Prepare rendering
// -------------------------------------------------------
void SimpleQuadNode::prepareRendering() {
	PR_START(m_NodeName)
	if ( m_VertexCounter > 0 ) {
		ds::TTCVertex* vb;
		WORD* ib;
		uint32 indexCounter = m_VertexCounter / 4 * 6;
		m_Renderer->lockBuffer(m_Handle,m_VertexCounter,indexCounter,(float**)&vb,(void**)&ib);
		char* vBuffer = m_DataBuffer;
		for ( uint32 i = 0; i < m_Index;++i ) {
			float rwh = (*(Plane*)vBuffer).v1.rwh;
			if ( rwh != 0.0f ) {
				vb->x = (*(Plane*)vBuffer).v1.x;
				vb->y = (*(Plane*)vBuffer).v1.y;
				vb->z = (*(Plane*)vBuffer).v1.z;
				vb->rwh = (*(Plane*)vBuffer).v1.rwh;
				vb->tu = (*(Plane*)vBuffer).v1.tu;
				vb->tv = (*(Plane*)vBuffer).v1.tv;
				vb->color = (*(Plane*)vBuffer).v1.color;
				++vb;
				vb->x = (*(Plane*)vBuffer).v2.x;
				vb->y = (*(Plane*)vBuffer).v2.y;
				vb->z = (*(Plane*)vBuffer).v2.z;
				vb->rwh = (*(Plane*)vBuffer).v2.rwh;
				vb->tu = (*(Plane*)vBuffer).v2.tu;
				vb->tv = (*(Plane*)vBuffer).v2.tv;
				vb->color = (*(Plane*)vBuffer).v2.color;
				++vb;
				vb->x = (*(Plane*)vBuffer).v3.x;
				vb->y = (*(Plane*)vBuffer).v3.y;
				vb->z = (*(Plane*)vBuffer).v3.z;
				vb->rwh = (*(Plane*)vBuffer).v3.rwh;
				vb->tu = (*(Plane*)vBuffer).v3.tu;
				vb->tv = (*(Plane*)vBuffer).v3.tv;
				vb->color = (*(Plane*)vBuffer).v3.color;
				++vb;
				vb->x = (*(Plane*)vBuffer).v4.x;
				vb->y = (*(Plane*)vBuffer).v4.y;
				vb->z = (*(Plane*)vBuffer).v4.z;
				vb->rwh = (*(Plane*)vBuffer).v4.rwh;
				vb->tu = (*(Plane*)vBuffer).v4.tu;
				vb->tv = (*(Plane*)vBuffer).v4.tv;
				vb->color = (*(Plane*)vBuffer).v4.color;
				++vb;

				/*
				m_Buffer->add((*(Plane*)vBuffer).v1);
				m_Buffer->add((*(Plane*)vBuffer).v2);
				m_Buffer->add((*(Plane*)vBuffer).v3);
				m_Buffer->add((*(Plane*)vBuffer).v4);
				*/
			}
			vBuffer += QUAD_SIZE;
		}		
		
		char* iBuffer = m_DataBuffer + m_StartIndices;
		for ( uint32 i = 0; i < indexCounter; ++i ) {
			//m_Buffer->addIndex((*(uint32*)iBuffer));
			*ib = (*(uint32*)iBuffer);
			++ib;
			iBuffer += INDEX_SIZE;
		}
		m_Renderer->unlockBuffer(m_Handle);
	}
	//m_Buffer->prepareData();
	PR_END(m_NodeName)
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void SimpleQuadNode::draw() {	
	
	//m_Buffer->render(m_VertexCounter/2);	
	m_Renderer->drawBuffer(m_Handle);
	
}

// -------------------------------------------------------
// Post rendering
// -------------------------------------------------------
void SimpleQuadNode::postRendering() {
	//m_Buffer->clear();
}

// -------------------------------------------------------
// Debug
// -------------------------------------------------------
void SimpleQuadNode::debug() {
	//LOGC(logINFO,"World") << "   Mesh2DNode -> Meshes: " << m_Meshes.size() << " Vertices: " << m_VertexCounter;
}


}
