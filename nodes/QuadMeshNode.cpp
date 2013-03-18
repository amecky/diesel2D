#include "QuadMeshNode.h"
#include "..\content\TextureAtlas.h"
#include "..\renderer\vertex_types.h"

namespace ds {

const uint32 QUAD_SIZE = 188;
const uint32 INDEX_SIZE = 4;
// ------------------------------------------------------------
//
// ------------------------------------------------------------
QuadMeshNode::QuadMeshNode(const char* name,Renderer* renderer,int material,uint32 maxQuads) : Node(name,renderer,material) , m_VertexCounter(0) , m_Index(0) {
	m_MaxVertices = 4 * maxQuads;
	m_MaxIndices = 6 * maxQuads;
	m_BufferSize = QUAD_SIZE * m_MaxVertices + INDEX_SIZE * m_MaxIndices;
	m_StartIndices = QUAD_SIZE * m_MaxVertices;
	m_DataBuffer = new char[m_BufferSize];
	//gEngine->getDrawCounter()->addMemory(m_BufferSize);
	LOGC(logINFO,"QuadMeshNode") << "creating new QuadMeshNode - buffer size " << m_BufferSize << " start index buffer " << m_StartIndices;
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


QuadMeshNode::~QuadMeshNode() {
	LOGC(logINFO,"QuadMeshNode") << "destructing QuadMeshNode " << getNodeName();
	//gEngine->getDrawCounter()->removeMemory(m_BufferSize);
	delete m_Buffer;
	delete m_DataBuffer;
}

uint32 QuadMeshNode::create(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color) {
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

	(*(Plane*)buffer).v1.x = -dx;
	(*(Plane*)buffer).v1.y = -dy;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v1.color = color;

	(*(Plane*)buffer).v2.x = dx;
	(*(Plane*)buffer).v2.y = -dy;
	(*(Plane*)buffer).v2.z = 0.0f;
	(*(Plane*)buffer).v2.rwh = 1.0f;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v2.color = color;

	(*(Plane*)buffer).v3.x = dx;
	(*(Plane*)buffer).v3.y = dy;
	(*(Plane*)buffer).v3.z = 0.0f;
	(*(Plane*)buffer).v3.rwh = 1.0f;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v3.color = color;

	(*(Plane*)buffer).v4.x = -dx;
	(*(Plane*)buffer).v4.y = dy;
	(*(Plane*)buffer).v4.z = 0.0f;
	(*(Plane*)buffer).v4.rwh = 1.0f;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
	(*(Plane*)buffer).v4.color = color;

	m_VertexCounter += 4;
	return idx;
}

uint32 QuadMeshNode::create(float dimX,float dimY,float rotation,const Vec2& pos,const char* taItem,const Color& color) {
	uint32 idx = m_Index;
	if ( !m_FreeList.empty()) {
		idx = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else {
		++m_Index;
	}
	float u1,v1,u2,v2;
	m_Renderer->getTextureCoordinates(m_Material,taItem,&u1,&v1,&u2,&v2);
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

	(*(Plane*)buffer).v1.x = -dx;
	(*(Plane*)buffer).v1.y = -dy;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v1.color = color;

	(*(Plane*)buffer).v2.x = dx;
	(*(Plane*)buffer).v2.y = -dy;
	(*(Plane*)buffer).v2.z = 0.0f;
	(*(Plane*)buffer).v2.rwh = 1.0f;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v2.color = color;

	(*(Plane*)buffer).v3.x = dx;
	(*(Plane*)buffer).v3.y = dy;
	(*(Plane*)buffer).v3.z = 0.0f;
	(*(Plane*)buffer).v3.rwh = 1.0f;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v3.color = color;

	(*(Plane*)buffer).v4.x = -dx;
	(*(Plane*)buffer).v4.y = dy;
	(*(Plane*)buffer).v4.z = 0.0f;
	(*(Plane*)buffer).v4.rwh = 1.0f;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
	(*(Plane*)buffer).v4.color = color;

	m_VertexCounter += 4;

	update(idx,1.0f,1.0f,rotation,pos);
	return idx;
}

uint32 QuadMeshNode::create(const Vec2& pos,const char* taItem,const Color& color) {
	uint32 idx = m_Index;
	if ( !m_FreeList.empty()) {
		idx = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else {
		++m_Index;
	}
	float u1,v1,u2,v2;
	m_Renderer->getTextureCoordinates(m_Material,taItem,&u1,&v1,&u2,&v2);
	float dx = m_Renderer->getAtlasItemWidth(m_Material,taItem) * 0.5f;
	float dy = m_Renderer->getAtlasItemHeight(m_Material,taItem) * 0.5f;
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).scaleX = 1.0f;	
	(*(Plane*)buffer).scaleY = 1.0f;
	(*(Plane*)buffer).rotation = 0.0f;
	(*(Plane*)buffer).dimX = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	(*(Plane*)buffer).dimY = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	(*(Plane*)buffer).x = pos.x;
	(*(Plane*)buffer).y = pos.y;

	(*(Plane*)buffer).v1.x = -dx;
	(*(Plane*)buffer).v1.y = -dy;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v1.color = color;

	(*(Plane*)buffer).v2.x = dx;
	(*(Plane*)buffer).v2.y = -dy;
	(*(Plane*)buffer).v2.z = 0.0f;
	(*(Plane*)buffer).v2.rwh = 1.0f;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v2.color = color;

	(*(Plane*)buffer).v3.x = dx;
	(*(Plane*)buffer).v3.y = dy;
	(*(Plane*)buffer).v3.z = 0.0f;
	(*(Plane*)buffer).v3.rwh = 1.0f;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v3.color = color;

	(*(Plane*)buffer).v4.x = -dx;
	(*(Plane*)buffer).v4.y = dy;
	(*(Plane*)buffer).v4.z = 0.0f;
	(*(Plane*)buffer).v4.rwh = 1.0f;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
	(*(Plane*)buffer).v4.color = color;

	m_VertexCounter += 4;
	update(idx,1.0f,1.0f,0.0f,pos);
	return idx;
}

// -------------------------------------------------------
// Create using 4 vertices
// -------------------------------------------------------
uint32 QuadMeshNode::create(const Vec2& p1,const Vec2& p2,const Vec2& p3,const Vec2& p4,const char* taItem,const Color& color) {
	uint32 idx = getIndex();
	float u1,v1,u2,v2;
	m_Renderer->getTextureCoordinates(m_Material,taItem,&u1,&v1,&u2,&v2);

	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).scaleX = 1.0f;	
	(*(Plane*)buffer).scaleY = 1.0f;
	(*(Plane*)buffer).rotation = 0.0f;
	(*(Plane*)buffer).dimX = m_Renderer->getAtlasItemWidth(m_Material,taItem);
	(*(Plane*)buffer).dimY = m_Renderer->getAtlasItemHeight(m_Material,taItem);
	(*(Plane*)buffer).x = p1.x;
	(*(Plane*)buffer).y = p2.y;

	(*(Plane*)buffer).v1.x = p1.x;
	(*(Plane*)buffer).v1.y = p1.y;
	(*(Plane*)buffer).v1.z = 0.0f;
	(*(Plane*)buffer).v1.rwh = 1.0f;
	(*(Plane*)buffer).v1.tu = u1;
	(*(Plane*)buffer).v1.tv = v1;
	(*(Plane*)buffer).v1.color = color;

	(*(Plane*)buffer).v2.x = p2.x;
	(*(Plane*)buffer).v2.y = p2.y;
	(*(Plane*)buffer).v2.z = 0.0f;
	(*(Plane*)buffer).v2.rwh = 1.0f;
	(*(Plane*)buffer).v2.tu = u2;
	(*(Plane*)buffer).v2.tv = v1;
	(*(Plane*)buffer).v2.color = color;

	(*(Plane*)buffer).v3.x = p3.x;
	(*(Plane*)buffer).v3.y = p3.y;
	(*(Plane*)buffer).v3.z = 0.0f;
	(*(Plane*)buffer).v3.rwh = 1.0f;
	(*(Plane*)buffer).v3.tu = u2;
	(*(Plane*)buffer).v3.tv = v2;
	(*(Plane*)buffer).v3.color = color;

	(*(Plane*)buffer).v4.x = p4.x;
	(*(Plane*)buffer).v4.y = p4.y;
	(*(Plane*)buffer).v4.z = 0.0f;
	(*(Plane*)buffer).v4.rwh = 1.0f;
	(*(Plane*)buffer).v4.tu = u1;
	(*(Plane*)buffer).v4.tv = v2;
	(*(Plane*)buffer).v4.color = color;

	m_VertexCounter += 4;
	return idx;
}

// -------------------------------------------------------
// Get index
// -------------------------------------------------------
uint32 QuadMeshNode::getIndex() {
	uint32 idx = m_Index;
	if ( !m_FreeList.empty()) {
		idx = m_FreeList.back();
		m_FreeList.pop_back();
	}
	else {
		++m_Index;
	}
	return idx;
}

void QuadMeshNode::update(uint32 index,float scaleX,float scaleY,float rotation,const Vec2& pos) {
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
		vector::rotate(p1,rotation);
		vector::rotate(p2,rotation);
		vector::rotate(p3,rotation);
		vector::rotate(p4,rotation);
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

void QuadMeshNode::update(uint32 index,const Vec2& pos) {
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

void QuadMeshNode::getPosition(uint32 idx,Vec2* outVec) {
	char* buffer = m_DataBuffer + idx * QUAD_SIZE;
	outVec->x = (*(Plane*)buffer).x;
	outVec->y = (*(Plane*)buffer).y;
}

void QuadMeshNode::setPosition(uint32 idx,const Vec2& v) {
	update(idx,v);
}

void QuadMeshNode::remove(uint32 index) {
	char* buffer = m_DataBuffer + QUAD_SIZE * index;
	(*(Plane*)buffer).v1.rwh = 0.0f;
	(*(Plane*)buffer).v2.rwh = 0.0f;		
	(*(Plane*)buffer).v3.rwh = 0.0f;
	(*(Plane*)buffer).v4.rwh = 0.0f;
	m_VertexCounter -= 4;
	m_FreeList.push_back(index);
}

// -------------------------------------------------------
// Get vertex position
// -------------------------------------------------------
void QuadMeshNode::getVertexPosition(uint32 idx,uint32 offset,Vec2* outVec) {
	assert(offset >= 0 );
	assert(offset < 4);
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
// Set vertex position directly
// -------------------------------------------------------
void QuadMeshNode::setVertexPosition(uint32 idx,uint32 offset,const Vec2& v) {
	assert(offset >= 0 );
	assert(offset < 4);
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
void QuadMeshNode::setTextureRect(uint32 idx,const Rect& textureRect,float textureSize) {
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

// -------------------------------------------------------
// Set texture coordinates directly
// -------------------------------------------------------
void QuadMeshNode::setTextureCoordinates(uint32 idx,float u1,float v1,float u2,float v2) {
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
void QuadMeshNode::setColor(uint32 idx,const Color& color) {
	char* buffer = m_DataBuffer + QUAD_SIZE * idx;
	(*(Plane*)buffer).v1.color = color;
	(*(Plane*)buffer).v2.color = color;
	(*(Plane*)buffer).v3.color = color;
	(*(Plane*)buffer).v4.color = color;
}

void QuadMeshNode::prepareRendering() {
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

void QuadMeshNode::draw() {
	m_Buffer->render(m_VertexCounter/2);	
}

void QuadMeshNode::postRendering() {
	m_Buffer->clear();
}

void QuadMeshNode::debug() {
	//LOGC(logINFO,"World") << "   Mesh2DNode -> Meshes: " << m_Meshes.size() << " Vertices: " << m_VertexCounter;
}


}