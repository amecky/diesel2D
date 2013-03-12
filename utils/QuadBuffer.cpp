#include "QuadBuffer.h"
#include "..\renderer\vertex_types.h"
namespace ds {

const uint32 QUAD_SIZE = 188;
const uint32 INDEX_SIZE = 4;
const uint32 TTVC_SIZE = 40;
// ------------------------------------------------------------
//
// ------------------------------------------------------------
QuadBuffer::QuadBuffer(Renderer* renderer,uint32 maxQuads) : m_Renderer(renderer) , m_Mtrl(-1) , m_VertexCounter(0) , m_Index(0) {
	m_MaxVertices = 4 * maxQuads;
	m_MaxIndices = 6 * maxQuads;
	m_BufferSize = QUAD_SIZE * m_MaxVertices + INDEX_SIZE * m_MaxIndices;
	m_StartIndices = QUAD_SIZE * m_MaxVertices;
	m_DataBuffer = new char[m_BufferSize];

	m_Handle = renderer->createBufferHandle(ds::PT_TRI,ds::VD_TTC,ds::GBT_BOTH,true);
	LOGC(logINFO,"QuadBuffer") << "creating new QuadBuffer - buffer size " << m_BufferSize << " start index buffer " << m_StartIndices;
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
}


QuadBuffer::~QuadBuffer() {
	LOGC(logINFO,"QuadBuffer") << "destructing QuadBuffer";	
	delete m_DataBuffer;
}

// -------------------------------------------------------
// Create new quad by texture rect
// -------------------------------------------------------
uint32 QuadBuffer::create(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color) {
	float u1,v1,u2,v2;
	ds::math::getTextureCoordinates(rect,textureSize,&u1,&v1,&u2,&v2);
	return create(dimX,dimY,rotation,pos,u1,v1,u2,v2,color);	
}

// -------------------------------------------------------
// Create new quad by texture positions
// -------------------------------------------------------
uint32 QuadBuffer::create(float dimX,float dimY,float rotation,const Vec2& pos,float u1,float v1,float u2,float v2,const Color& color) {
	uint32 idx = m_Index;
	if ( !m_FreeList.empty()) {
		idx = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else {
		++m_Index;
	}
	float dx = dimX * 0.5f;
	float dy = dimY * 0.5f;
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).scaleX = 1.0f;	
	(*(Plane*)buffer).scaleY = 1.0f;
	(*(Plane*)buffer).rotation = rotation;
	(*(Plane*)buffer).dimX = dimX;
	(*(Plane*)buffer).dimY = dimY;
	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	setVertex(idx,0,-dx,-dy,u1,v1,color);
	setVertex(idx,1,dx,-dy,u2,v1,color);
	setVertex(idx,2,dx,dy,u2,v2,color);
	setVertex(idx,3,-dx,dy,u1,v2,color);
	m_VertexCounter += 4;
	update(idx,1.0f,1.0f,rotation,pos);
	return idx;
}

// -------------------------------------------------------
// Set specific vertex
// -------------------------------------------------------
void QuadBuffer::setVertex(int idx,int offset,float x,float y,float tu,float tv,const Color& color) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	buffer += offset * TTVC_SIZE;
	(*(Plane*)buffer).v1.x = x;
	(*(Plane*)buffer).v1.y = y;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = tu;
	(*(Plane*)buffer).v1.tv = tv;
	(*(Plane*)buffer).v1.color = color;
}

// -------------------------------------------------------
// Update quad
// -------------------------------------------------------
void QuadBuffer::update(uint32 index,float scaleX,float scaleY,float rotation,const Vec2& pos) {
	char* buffer = m_DataBuffer + index * QUAD_SIZE;
	(*(Plane*)buffer).scaleX = scaleX;	
	(*(Plane*)buffer).scaleY = scaleY;
	(*(Plane*)buffer).rotation = rotation;
	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	float dimX = (*(Plane*)buffer).dimX;
	float dimY = (*(Plane*)buffer).dimY;

	float hdx = dimX * scaleX * 0.5f;
	float hdy = dimY * scaleY * 0.5f;

	Vec2 p1,p2,p3,p4;

	p1.x = - hdx;
	p1.y = - hdy;
	p2.x = hdx;
	p2.y = - hdy;
	p3.x = hdx;
	p3.y = hdy;
	p4.x = - hdx;
	p4.y = hdy;	

	if ( rotation != 0 ) {
		p1.rotate(rotation);
		p2.rotate(rotation);
		p3.rotate(rotation);
		p4.rotate(rotation);
	}
	p1 += pos;
	p2 += pos;
	p3 += pos;
	p4 += pos;

	(*(Plane*)buffer).v1.x = p1.x;
	(*(Plane*)buffer).v1.y = p1.y;
	(*(Plane*)buffer).v2.x = p2.x;
	(*(Plane*)buffer).v2.y = p2.y;
	(*(Plane*)buffer).v3.x = p3.x;
	(*(Plane*)buffer).v3.y = p3.y;
	(*(Plane*)buffer).v4.x = p4.x;
	(*(Plane*)buffer).v4.y = p4.y;
}

// -------------------------------------------------------
// Update quad position only
// -------------------------------------------------------
void QuadBuffer::update(uint32 index,const Vec2& pos) {
	char* buffer = m_DataBuffer + index * QUAD_SIZE;
	float scaleX = (*(Plane*)buffer).scaleX;	
	float scaleY = (*(Plane*)buffer).scaleY;
	float rotation = (*(Plane*)buffer).rotation;

	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	float m00 = scaleX * cos(rotation);
	float m01 = scaleX * -sin(rotation);
	float m10 = scaleY * sin(rotation);
	float m11 = scaleY * cos(rotation);

	float dx = (*(Plane*)buffer).dimX * 0.5f;
	float dy = (*(Plane*)buffer).dimY * 0.5f;

	float x = -dx * m00 - dy * m01 + pos.x;
	float y = -dx * m10 - dy * m11 + pos.y;
	(*(Plane*)buffer).v1.x = x;
	(*(Plane*)buffer).v1.y = y;

	x = dx * m00 - dy * m01 + pos.x;
	y = dx * m10 - dy * m11 + pos.y;
	(*(Plane*)buffer).v2.x = x;
	(*(Plane*)buffer).v2.y = y;

	x = dx * m00 + dy * m01 + pos.x;
	y = dx * m10 + dy * m11 + pos.y;
	(*(Plane*)buffer).v3.x = x;
	(*(Plane*)buffer).v3.y = y;

	x = -dx * m00 + dy * m01 + pos.x;
	y = -dx * m10 + dy * m11 + pos.y;
	(*(Plane*)buffer).v4.x = x;
	(*(Plane*)buffer).v4.y = y;
}

// -------------------------------------------------------
// Get position of quad
// -------------------------------------------------------
void QuadBuffer::getPosition(uint32 idx,Vec2* outVec) {
	char* buffer = m_DataBuffer + idx * QUAD_SIZE;
	outVec->x = (*(Plane*)buffer).x;
	outVec->y = (*(Plane*)buffer).y;
}

// -------------------------------------------------------
// Set position of quad
// -------------------------------------------------------
void QuadBuffer::setPosition(uint32 idx,const Vec2& v) {
	update(idx,v);
}

// -------------------------------------------------------
// Remove quad
// -------------------------------------------------------
void QuadBuffer::remove(uint32 index) {
	char* buffer = m_DataBuffer + QUAD_SIZE * index;
	for ( int i = 0; i < 4; ++i ) {
		(*(Plane*)buffer).v1.rwh = 0.0f;
		buffer += TTVC_SIZE;
	}
	m_VertexCounter -= 4;
	m_FreeList.push_back(index);
}


// -------------------------------------------------------
// Get specific vertex position
// -------------------------------------------------------
void QuadBuffer::getPosition(uint32 idx,uint32 offset,Vec2* outVec) {
	assert(offset>= 0);
	assert(offset<4);
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	buffer += offset * TTVC_SIZE;
	outVec->x = (*(Plane*)buffer).v1.x;
	outVec->y = (*(Plane*)buffer).v1.y;
}

// -------------------------------------------------------
// set specific vertex position
// -------------------------------------------------------
void QuadBuffer::setPosition(uint32 idx,uint32 offset,const Vec2& v) {
	assert(offset>= 0);
	assert(offset<4);
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	buffer += offset * TTVC_SIZE;
	(*(Plane*)buffer).v1.x = v.x;
	(*(Plane*)buffer).v1.y = v.y;
}

// --------------------------------------------------
// Set texture rect
// --------------------------------------------------
void QuadBuffer::setTextureRect(uint32 idx,const Rect& textureRect,float textureSize) {
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
void QuadBuffer::setColor(uint32 idx,const Color& color) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	for ( int i = 0; i < 4; ++i ) {
		(*(Plane*)buffer).v1.color = color;
		buffer += TTVC_SIZE;
	}
}

// -------------------------------------------------------
// Prepare buffer
// -------------------------------------------------------
void QuadBuffer::prepareBuffer() {
	if ( m_VertexCounter > 0 ) {
		WORD* ib;
		TTCVertex* vb;
		int vc = m_VertexCounter;
		int ic = m_VertexCounter / 4 * 6;
		m_Renderer->lockBuffer(m_Handle,vc,ic,(float**)&vb,(void**)&ib);
		char* vBuffer = m_DataBuffer;
		for ( uint32 i = 0; i < m_Index;++i ) {
			float rwh = (*(Plane*)vBuffer).v1.rwh;
			if ( rwh != 0.0f ) {
				*vb = (*(Plane*)vBuffer).v1;
				++vb;
				*vb = (*(Plane*)vBuffer).v2;
				++vb;
				*vb = (*(Plane*)vBuffer).v3;
				++vb;
				*vb = (*(Plane*)vBuffer).v4;
				++vb;
			}
			vBuffer += QUAD_SIZE;
		}		
		uint32 indexCounter = m_VertexCounter / 4 * 6;
		char* iBuffer = m_DataBuffer + m_StartIndices;
		for ( uint32 i = 0; i < indexCounter; ++i ) {
			*ib = (*(uint32*)iBuffer);
			++ib;
			iBuffer += INDEX_SIZE;
		}
		m_Renderer->unlockBuffer(m_Handle);
	}
	
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void QuadBuffer::draw() {
	if ( m_Mtrl != -1 ) {
		m_Renderer->applyMaterial(m_Mtrl);
	}
	prepareBuffer();
	m_Renderer->drawBuffer(m_Handle);
}

}
