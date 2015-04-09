#pragma once
#include "..\dxstdafx.h"
#include "vertex_types.h"

namespace ds {

	struct VertexBuffer {

		LPDIRECT3DVERTEXBUFFER9 buffer;
		int vertexDeclaration;
		int size;
		int vertexSize;
		int num;

		VertexBuffer() : buffer(0), size(0), vertexDeclaration(-1) , num(0) {}

	};

	struct IndexBuffer {

		LPDIRECT3DINDEXBUFFER9 buffer;
		int size;
		int num;
	};

	namespace buffer {

		void create(VertexBuffer& buffer, int vertexType, int size, bool dynamic = true);

		void create(IndexBuffer& buffer, int size);

		void buildQuadIndices(IndexBuffer& buffer, int size);

	}

}