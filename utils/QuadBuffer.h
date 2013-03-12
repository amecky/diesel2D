#pragma once
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\IndexVertexBuffer.h"
#include "..\renderer\render_types.h"

namespace ds {

// -----------------------------------------------------------
// QuadBuffer
// -----------------------------------------------------------
class QuadBuffer {

typedef std::vector<uint32> FreeList;

public:
	QuadBuffer(Renderer* renderer,uint32 maxQuads);
	virtual ~QuadBuffer(void);
	void draw();
	uint32 create(float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,const Color& color = Color::WHITE);
	uint32 create(float dimX,float dimY,float rotation,const Vec2& pos,float u1,float v1,float u2,float v2,const Color& color = Color::WHITE);
	void update(uint32 index,float scaleX,float scaleY,float rotation,const Vec2& pos);
	void update(uint32 index,const Vec2& pos);
	void remove(uint32 index);
	void getPosition(uint32 idx,uint32 offset,Vec2* outVec);
	void setPosition(uint32 idx,uint32 offset,const Vec2& v);
	void getPosition(uint32 idx,Vec2* outVec);
	void setPosition(uint32 idx,const Vec2& v);	
	void setColor(uint32 idx,const Color& color);
	void setTextureRect(uint32 idx,const Rect& textureRect,float textureSize);
private:
	void setVertex(int idx,int offset,float x,float y,float tu,float tv,const Color& color);
	void prepareBuffer();
	ds::Renderer* m_Renderer;
	int m_Handle;
	int m_Mtrl;
	uint32 m_MaxVertices;
	uint32 m_MaxIndices;
	uint32 m_VertexCounter;
	FreeList m_FreeList;
	uint32 m_Index;
	char* m_DataBuffer;
	uint32 m_BufferSize;
	uint32 m_StartIndices;
};

}

