#pragma once
#include "math_types.h"

ds::mat3 operator * (const ds::mat3& m1,const ds::mat3& m2);

ds::Vec3 operator * (const ds::mat3& m,const ds::Vec3& v);

namespace ds {

	namespace matrix {

		mat3 indentity();

		mat3 srt(float scale,float rotation,float x,float y);

		mat3 srt(float scaleX,float scaleY,float rotation,float x,float y);

		Vec3 mul(const mat3& m,const Vec3& v);

		Vec2 mul(const mat3& m,const Vec2& v);
	}

}
