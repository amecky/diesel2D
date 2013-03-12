#include "buffer_types.h"
#include "vertex_types.h"
#include "..\utils\GameMath.h"

namespace ds {

void GeometryBuffer::initialize() {
	size = 0;
	maxSize = 0;
	used = 0;
	userCounter = 0;
	vertexBuffer = 0;
}

namespace buffer {

void createQuad(TTCVertex* vb,WORD* ib,float dimX,float dimY,const ds::Vec2& pos,const ds::Rect& rect,float textureSize,int idxOffset,const Color& color,QuadAlignment align) {
	float u1,v1,u2,v2;
	math::getTextureCoordinates(rect,textureSize,&u1,&v1,&u2,&v2);
	float x1 = pos.x;
	float y1 = pos.y;

	if ( align == QA_CENTERED ) {
		x1 -= dimX * 0.5f;
		y1 -= dimY * 0.5f;
	}

	vb->x = x1;
	vb->y = y1;
	vb->z = 0.0f;
	vb->rwh = 1.0f;
	vb->tu = u1;
	vb->tv = v1;
	vb->color = color;
	++vb;

	vb->x = x1 + dimX;
	vb->y = y1;
	vb->z = 0.0f;
	vb->rwh = 1.0f;
	vb->tu = u2;
	vb->tv = v1;
	vb->color = color;
	++vb;

	vb->x = x1 + dimX;
	vb->y = y1 + dimY;
	vb->z = 0.0f;
	vb->rwh = 1.0f;
	vb->tu = u2;
	vb->tv = v2;
	vb->color = color;
	++vb;
	vb->x = x1;
	vb->y = y1 + dimY;
	vb->z = 0.0f;
	vb->rwh = 1.0f;
	vb->tu = u1;
	vb->tv = v2;
	vb->color = color;
	++vb;

	static WORD ind[] = {0,1,2,2,3,0};

	for ( int i = 0; i < 6; ++i ) {
		*ib = ind[i]+idxOffset;
		++ib;
	}
}

}

}
