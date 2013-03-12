#pragma once
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\IndexVertexBuffer.h"
#include "..\renderer\render_types.h"

namespace ds {

// -----------------------------------------------------------
//
// -----------------------------------------------------------
class SimpleQuadNode : public Node {

typedef std::vector<uint32> FreeList;

public:
	SimpleQuadNode(const char* nodeName,Renderer* renderer,int material,uint32 maxQuads);
	virtual ~SimpleQuadNode(void);
	void init();
	void prepareRendering();
	void postRendering();
	void draw();
	uint32 create(float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,const Color& color = Color::WHITE);
	uint32 create(const Vec2& pos,const char* taItem,const Color& color = Color::WHITE);
	uint32 create(float dimX,float dimY,const Vec2& pos,const char* taItem,const Color& color = Color::WHITE);
	void update(uint32 index,float scaleX,float scaleY,const Vec2& pos);
	void update(uint32 index,const Vec2& pos);
	void remove(uint32 index);
	void getPosition(uint32 idx,uint32 offset,Vec2* outVec);
	void setPosition(uint32 idx,uint32 offset,const Vec2& v);
	void getPosition(uint32 idx,Vec2* outVec);
	void setPosition(uint32 idx,const Vec2& v);
	void setColor(uint32 idx,const Color& color);
	void setTextureRect(uint32 idx,const char* taItem);
	void setTextureRect(uint32 idx,const Rect& textureRect,float textureSize);
	void setTextureCoordinates(uint32 idx,float u1,float v1,float u2,float v2);
	void debug();
protected:
	uint32 create(float dimX,float dimY,const Vec2& pos,float u1,float v1,float u2,float v2,const Color& color = Color::WHITE);
private:
	//TTCBuffer* m_Buffer;
	int m_Handle;
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



