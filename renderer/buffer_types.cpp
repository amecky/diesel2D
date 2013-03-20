#include "buffer_types.h"
#include "vertex_types.h"
#include "..\utils\GameMath.h"
#include "..\utils\Log.h"

namespace ds {

	const float VP_ARRAY[] = {
		-0.5f,0.5f, 0.5f,0.5f, 0.5f,-0.5f, -0.5f,-0.5f
	};
	int UV_LOOKUP[] = {0,1,2,1,2,3,0,3};
	const WORD IDX_AR[] = {0,1,2,2,3,0};

	void GeometryBuffer::initialize() {
		size = 0;
		maxSize = 0;
		used = 0;
		userCounter = 0;
		vertexBuffer = 0;
	}

	namespace buffer {

		// -------------------------------------------------------
		// Create quad
		// -------------------------------------------------------
		void createQuad(TTCVertex* vb,WORD* ib,float dimX,float dimY,const ds::Vec2& pos,const ds::Rect& rect,float textureSize,int idxOffset,const Color& color,QuadAlignment align) {
			createQuad(vb,ib,dimX,dimY,0.0f,pos,rect,textureSize,idxOffset,color,align);	
		}

		// -------------------------------------------------------
		// Create quad
		// -------------------------------------------------------
		void createQuad(TTCVertex* vb,WORD* ib,float dimX,float dimY,float rotation,const ds::Vec2& pos,const ds::Rect& rect,float textureSize,int idxOffset,const Color& color,QuadAlignment align) {
			float uv[4];
			math::getTextureCoordinates(rect,textureSize,&uv[0],&uv[1],&uv[2],&uv[3]);
			Vec2 p;
			Vec2 np = pos;
			
			if ( align == QA_TOP_LEFT ) {
				np.x += dimX * 0.5f;
				np.y += dimY * 0.5f;
			}
			
			mat3 srt = matrix::srt(1.0f,1.0f,rotation,0.0f,0.0f);
			for ( int i = 0; i < 4; ++i ) {
				p.x = VP_ARRAY[i * 2] * dimX;
				p.y = VP_ARRAY[i * 2 + 1] * dimY;
				p = matrix::mul(srt,p);		
				p = p + np;
				vb->x = p.x;
				vb->y = p.y;
				vb->z = 0.0f;
				vb->rwh = 1.0f;
				int tul = UV_LOOKUP[i*2];
				vb->tu = uv[tul];
				int tvl = UV_LOOKUP[i*2+1];
				vb->tv = uv[tvl];
				vb->color = color;
				++vb;
			}	
			for ( int i = 0; i < 6; ++i ) {
				*ib = IDX_AR[i]+idxOffset;
				++ib;
			}
		}

	}

}
