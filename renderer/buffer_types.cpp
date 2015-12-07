#include "buffer_types.h"
#include "vertex_types.h"
#include "..\math\math_types.h"
#include "..\utils\Log.h"
#include "..\renderer\graphics.h"

namespace ds {
	
	namespace buffer {

		void create(VertexBuffer& buffer,int vertexType, int size, bool dynamic) {
			VDStruct vd = renderer::getVertexDeclaration(vertexType);
			buffer.vertexSize = vd.vertexSize;
			buffer.vertexDeclaration = vertexType;
			D3DPOOL pool = D3DPOOL_DEFAULT;
			DWORD usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
			LOG << "creating new vertex buffer - size: " << size << " vd size: " << buffer.vertexSize;
			HR(renderer::getDevice()->CreateVertexBuffer(size * buffer.vertexSize, usage, 0, pool, &buffer.buffer, NULL));
		}

		void create(IndexBuffer& buffer, int size) {
			D3DPOOL pool = D3DPOOL_MANAGED;
			DWORD usage = D3DUSAGE_WRITEONLY;
			HR(renderer::getDevice()->CreateIndexBuffer(size * sizeof(WORD), usage, D3DFMT_INDEX16, pool, &buffer.buffer, NULL));
			LOG << "new IndexBuffer created size: " << size;
		}

		void buildQuadIndices(IndexBuffer& buffer, int size) {
			int max = size / 6;
			WORD* indexBuffer;
			HR(buffer.buffer->Lock(0, size * sizeof(WORD), (void**)&indexBuffer, 0));
			for (uint32 i = 0; i < max; ++i) {
				*indexBuffer = i * 4 + 0;
				++indexBuffer;
				*indexBuffer = i * 4 + 1;
				++indexBuffer;
				*indexBuffer = i * 4 + 2;
				++indexBuffer;
				*indexBuffer = i * 4 + 2;
				++indexBuffer;
				*indexBuffer = i * 4 + 3;
				++indexBuffer;
				*indexBuffer = i * 4 + 0;
				++indexBuffer;
			}
			HR(buffer.buffer->Unlock());
		}

	}

}
