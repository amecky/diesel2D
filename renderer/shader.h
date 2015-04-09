#pragma once
#include "render_types.h"
#include "..\math\math_types.h"
#include "Renderer.h"

namespace ds {

	namespace shader {

		int createTransformedTextureColorShader(Renderer* renderer,int textureId);
		int createPTCShader(int textureId);

		int createBloomShader(Renderer* renderer,int textureID,float threshold);
		int createBlurShader(Renderer* renderer,int textureID);
		int createBloomCombineShader(Renderer* renderer,int colorTextureID,int bloomTextureID);

		int createCombineLightShader(Renderer* renderer, int lightTextureID, int colorTextureID);

		bool setVector3f(Shader& shader,const char* name,const Vector3f& v);
		bool setVector2f(Shader& shader, const char* name, const Vector2f& v);
		bool setValue(Shader& shader,const char* name,void* data,UINT size);
		bool setTexture(Shader& shader,const char* name,int textureID);
		bool setColor(Shader& shader,const char* name,const Color& color);
		bool setFloat(Shader& shader,const char* name,float value);
		D3DXHANDLE findHandle(const Shader& shader,const char* name);
	}
}