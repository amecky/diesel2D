#pragma once
#include "render_types.h"
#include "..\math\math_types.h"
#include "Renderer.h"

namespace ds {

	namespace shader {

		int createTransformedTextureColorShader(Renderer* renderer,int textureId);

		bool setVec3(Shader& shader,const char* name,const Vec3& v);
		bool setValue(Shader& shader,const char* name,void* data,UINT size);
		bool setTexture(Shader& shader,const char* name,Renderer* renderer,int textureID);
		bool setColor(Shader& shader,const char* name,const Color& color);
		bool setFloat(Shader& shader,const char* name,float value);
		D3DXHANDLE findHandle(const Shader& shader,const char* name);
	}
}