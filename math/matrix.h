#pragma once
#include "math_types.h"
#include <d3dx9math.h>

ds::mat3 operator * (const ds::mat3& m1,const ds::mat3& m2);

ds::Vec3 operator * (const ds::mat3& m,const ds::Vec3& v);

ds::Vec2 operator * (const ds::mat3& m,const ds::Vec2& v);

ds::Vec4 operator * (const ds::mat4& m,const ds::Vec4& v);

ds::Vec3 operator * (const ds::mat4& m,const ds::Vec3& v);

ds::mat4 operator * (const ds::mat4& m1,const ds::mat4& m2);

ds::mat4 operator * (const ds::mat4& m,float v);

namespace ds {

	namespace matrix {

		mat3 identity();

		mat3 srt(float scale,float rotation,float x,float y);

		mat3 srt(float scaleX,float scaleY,float rotation,float x,float y);

		Vec3 mul(const mat3& m,const Vec3& v);

		Vec2 mul(const mat3& m,const Vec2& v);

		mat4 m4identity();

		mat3 mat3Transform(const Vec2& pos);

		mat3 mat3Translation(float x,float y);

		mat3 mat3Scale(const Vec2& pos);

		mat3 mat3RotationY(float angle);

		mat3 mat3RotationX(float angle);

		mat3 mat3RotationZ(float angle);

		mat4 mat4Scale(float scaleX,float scaleY,float scaleZ);

		mat4 mat4Transform(const Vec3& pos);

		mat4 mat4Rotation(const Vec3& v,float angle);

		mat4 mat4RotationZ(float angle);

		mat4 mat4RotateZXY(float angleX,float angleY,float angleZ);

		mat4 mat4PerspectiveFovLH(float fovy,float aspect,float zn,float zf);

		mat4 mat4LookAtLH(const Vec3& eye,const Vec3& lookAt,const Vec3& up);	
				
		mat4 mat4OrthoLH(float w,float h,float zn,float zf);

		mat4 mat4Transpose(const mat4& m);

		mat4 mat4Inverse(const mat4& m);

		D3DXMATRIX convert(const mat4& m);

		mat3 rotationAlign(const Vec2& d);

		mat3 rotationAlign(const Vec3& d);

		mat3 rotation(const Vec3& d,const Vec3& z);

		mat3 rotationAxisCosSin(const Vec3& v,float co,float si );
	}

}
