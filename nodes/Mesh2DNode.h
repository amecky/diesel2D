#pragma once
#include "Node.h"
#include <vector>
#include "..\renderer\IndexVertexBuffer.h"
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"

namespace ds {

// -----------------------------------------------------------
//
// -----------------------------------------------------------
class Mesh2D {

public:
	Mesh2D(uint32 maxVertices,uint32 maxIndices);
	virtual ~Mesh2D();
	// virtual methods
	virtual void update(float elapsed) {}
	uint32 fillBuffer(TTCBuffer* buffer,uint32 vertexStart);
	void setVertex(uint32 index,float x,float y);
	void setUV(uint32 index,float u,float v);
	void setUV(uint32 index,int offset,const char* taItem);
	void setIndex(uint32 index,uint32 idx);
	void setColor(uint32 index,const ds::Color& color);
	void setVertexCounter(uint32 vc) {
		m_VertexCounter = vc;
	}
	void setCounter(uint32 vertices,uint32 indices,uint32 triangleCount) {
		m_VertexCounter = vertices;
		m_IndexCounter = indices;
		m_TriangleCounter = triangleCount;
	}
	uint32 getVertexCounter() {
		return m_VertexCounter;
	}
	bool isActive() const {
		return m_Active;
	}
	void setActive(bool active) {
		m_Active = active;
	}
private:
	bool m_Active;
	uint32 m_MaxVertices;
	uint32 m_MaxIndices;
	uint32 m_IndexCounter;
	uint32 m_TriangleCounter;
	uint32 m_VertexCounter;
	char* m_Buffer;
	uint32 m_BufferSize;
	uint32 m_StartIndices;
};

// -----------------------------------------------------------
//
// -----------------------------------------------------------
class Mesh2DNode : public ds::Node {

typedef std::vector<Mesh2D*> Meshes;

public:
	Mesh2DNode(const char* name,Renderer* renderer,int material,uint32 maxVertices,uint32 maxIndices);
	virtual ~Mesh2DNode(void);
	void prepareRendering();
	void postRendering();
	void draw();
	void update(float elapsed);
	Mesh2D* create(uint32 maxVertices,uint32 maxIndices);
	void add(Mesh2D* mesh);
	void clear();
	void debug();
private:
	Meshes m_Meshes;
	TTCBuffer* m_Buffer;
	uint32 m_MaxVertices;
	uint32 m_MaxIndices;
	uint32 m_VertexCounter;
};

}

