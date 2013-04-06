#pragma once
#include "..\dxstdafx.h"
#include "vertex_types.h"
#include "..\utils\Quad.h"
#include "..\content\TextureAtlas.h"

namespace ds {

enum GeoBufferType {
	GBT_VERTEX = 0,
	GBT_INDEX  = 1,
	GBT_BOTH   = 2
};

enum PrimitiveType {
	PT_IDX = 0,
	PT_TRI = 1
};

// -------------------------------------------------------
// Geometry buffer
// -------------------------------------------------------
struct GeometryBuffer {
	GeoBufferType type;
	uint32 maxSize;
	uint32 size;
	uint32 used;
	uint32 userCounter;
	int vertexDefinition;
	bool dynamic;
	PrimitiveType primitiveType;
	union {
		LPDIRECT3DINDEXBUFFER9 indexBuffer;
		LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
	};
	void initialize();
};

// -------------------------------------------------------
// Buffer reference
// -------------------------------------------------------
struct BufferReference {
	uint32 bufferIdx;
	int start;
	int count;
};
// -------------------------------------------------------
// Buffer handle
// -------------------------------------------------------
struct GeoBufferHandle {
	int locked;
	BufferReference vBufferRef;
	BufferReference iBufferRef;
	PrimitiveType primitiveType;
	int used;
	bool dynamic;
	GeoBufferType bufferType;
	int vertexDefinition;
};

namespace buffer {

	void createQuad(TTCVertex* vb,WORD* ib,const Vec2& pos,const Rect& rect,float textureSize,int idxOffset,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
	void createQuad(TTCVertex* vb,WORD* ib,const Vec2& pos,const TAtlas& atlas,const char* name,int idxOffset,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
	void createQuad(TTCVertex* vb,WORD* ib,float dimX,float dimY,const Vec2& pos,const TAtlas& atlas,const char* name,int idxOffset,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
	void createQuad(TTCVertex* vb,WORD* ib,float dimX,float dimY,const Vec2& pos,const Rect& rect,float textureSize,int idxOffset,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);
	void createQuad(TTCVertex* vb,WORD* ib,float dimX,float dimY,float rotation,const Vec2& pos,const Rect& rect,float textureSize,int idxOffset,const Color& color = Color::WHITE,QuadAlignment align = QA_CENTERED);

}

}