#pragma once
#include "math_types.h"

namespace ds {

	namespace matrix {

		mat3 indentity();

		mat3 srt(float scale,float rotation,float x,float y);

		mat3 srt(float scaleX,float scaleY,float rotation,float x,float y);

		Vec3 mul(const mat3& m,const Vec3& v);
	}

}
