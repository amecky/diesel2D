#pragma once
#include "Color.h"
#include "Quad.h"
#include "..\renderer\Renderer.h"
#include "..\renderer\buffer_types.h"
#include "..\content\TextureAtlas.h"
#include "..\math\math_types.h"

namespace ds {

struct BufferQuad {

	float dimX;
	float dimY;
	float rotation;
	Vec2 pos;
	Rect rect;
	float textureSize;
	Color color;
	QuadAlignment align;

	BufferQuad() : dimX(0.0f) , dimY(0.0f) , rotation(0.0f) , pos(0,0) , rect(0,0,0,0) , textureSize(1024.0f) , color(Color::WHITE) , align(QA_CENTERED) {}
};

template <class T>
class BufferHelper {

public:
	BufferHelper(Renderer*renderer,float textureSize = 1024.0f,const Color& color = Color::WHITE,QuadAlignment align = ds::QA_CENTERED) 
		: m_Renderer(renderer) , m_TextureSize(1024.0f) , m_Color(color) , m_Alignment(align) , m_IdxOffset(0) {}
	~BufferHelper() {}
	void lock(int handle,int vertexCount,int indexCount);
	void unlock();
	void addQuad(const BufferQuad& quad);
	void addQuad(const Vec2& pos,const Rect& rect);
	void addQuad(const Vec2& pos,const TAtlas& atlas,const char* name);
	void addQuad(float dimX,float dimY,const Vec2& pos,const Rect& rect);
	void addQuad(float dimX,float dimY,const Vec2& pos,const TAtlas& atlas,const char* name);
	void addQuad(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
	void addQuad(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
private:
	T* m_VB;
	WORD* m_IB;
	int m_IdxOffset;
	Renderer* m_Renderer;
	int m_Handle;
	float m_TextureSize;
	Color m_Color;
	QuadAlignment m_Alignment;
};

template <class T>
void BufferHelper<T>::lock(int handle,int vertexCount,int indexCount) {
	m_Handle = handle;
	m_Renderer->lockBuffer(handle,vertexCount,indexCount,(float**)&m_VB,(void**)&m_IB);
}

template<class T>
void BufferHelper<T>::unlock() {
	m_Renderer->unlockBuffer(m_Handle);
}

template<class T>
void BufferHelper<T>::addQuad(const BufferQuad& quad) {
	buffer::createQuad(m_VB,m_IB,quad.dimX,quad.dimY,quad.rotation,quad.pos,quad.rect,quad.textureSize,m_IdxOffset,quad.color,quad.align);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(const Vec2& pos,const Rect& rect) {
	buffer::createQuad(m_VB,m_IB,pos,rect,m_TextureSize,m_IdxOffset,m_Color,m_Alignment);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(const Vec2& pos,const TAtlas& atlas,const char* name) {
	buffer::createQuad(m_VB,m_IB,pos,atlas,name,m_IdxOffset,m_Color,m_Alignment);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(float dimX,float dimY,const Vec2& pos,const TAtlas& atlas,const char* name) {
	buffer::createQuad(m_VB,m_IB,dimX,dimY,pos,atlas,name,m_IdxOffset,m_Color,m_Alignment);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(float dimX,float dimY,const Vec2& pos,const Rect& rect) {
	buffer::createQuad(m_VB,m_IB,dimX,dimY,pos,rect,m_TextureSize,m_IdxOffset,m_Color,m_Alignment);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color,QuadAlignment align) {
	buffer::createQuad(m_VB,m_IB,dimX,dimY,pos,rect,textureSize,m_IdxOffset,color,align);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color,QuadAlignment align) {
	buffer::createQuad(m_VB,m_IB,dimX,dimY,rotation,pos,rect,textureSize,m_IdxOffset,color,align);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

}