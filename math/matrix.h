#pragma once
#include "math_types.h"

ds::mat3 operator * (const ds::mat3& m1,const ds::mat3& m2);

ds::Vec3 operator * (const ds::mat3& m,const ds::Vec3& v);

ds::Vec4 operator * (const ds::mat4& m,const ds::Vec4& v);

ds::Vec3 operator * (const ds::mat4& m,const ds::Vec3& v);

ds::mat4 operator * (const ds::mat4& m1,const ds::mat4& m2);

namespace ds {

	namespace matrix {

		mat3 identity();

		mat3 srt(float scale,float rotation,float x,float y);

		mat3 srt(float scaleX,float scaleY,float rotation,float x,float y);

		Vec3 mul(const mat3& m,const Vec3& v);

		Vec2 mul(const mat3& m,const Vec2& v);

		mat4 m4identity();

		mat3 mat3RotationY(float angle);

		mat3 mat3RotationX(float angle);

		mat3 mat3RotationZ(float angle);

		mat4 mat4Rotation(const Vec3& v,float angle);
	}

}
