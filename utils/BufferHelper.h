#pragma once
#include "Color.h"
#include "Quad.h"
#include "..\renderer\Renderer.h"

namespace ds {

template <class T>
class BufferHelper {

public:
	BufferHelper(Renderer*renderer) : m_Renderer(renderer) , m_IdxOffset(0) {}
	~BufferHelper() {}
	void lock(int handle,int vertexCount,int indexCount);
	void unlock();
	void addQuad(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
	void addQuad(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
private:
	T* m_VB;
	WORD* m_IB;
	int m_IdxOffset;
	Renderer* m_Renderer;
	int m_Handle;
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
void BufferHelper<T>::addQuad(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color,QuadAlignment align) {
	ds::buffer::createQuad(m_VB,m_IB,dimX,dimY,pos,rect,textureSize,m_IdxOffset,color,align);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

template<class T>
void BufferHelper<T>::addQuad(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color,QuadAlignment align) {
	ds::buffer::createQuad(m_VB,m_IB,dimX,dimY,rotation,pos,rect,textureSize,m_IdxOffset,color,align);
	m_VB += 4;
	m_IB += 6;
	m_IdxOffset += 4;
}

}