#include "shader.h"
#include "..\utils\StringUtils.h"

namespace ds {

	namespace shader {

		bool setVec3(Shader& shader,const char* name,const Vec3& v) {
			D3DXHANDLE handle = findHandle(shader,name);
			if ( handle != 0 ) {
				shader.m_FX->SetValue(handle,&v,sizeof(Vec3));
				return true;
			}
			return false;
		}

		bool setValue(Shader& shader,const char* name,void* data,UINT size) {
			D3DXHANDLE handle = findHandle(shader,name);
			if ( handle != 0 ) {
				shader.m_FX->SetValue(handle,&data,size);
				return true;
			}
			return false;
		}

		bool setTexture(Shader& shader,const char* name,Renderer* renderer,int textureID) {
			D3DXHANDLE handle = findHandle(shader,name);
			if ( handle != 0 ) {
				shader.m_FX->SetTexture(handle,renderer->getDirectTexture(textureID));
				return true;
			}
			return false;
		}

		bool setColor(Shader& shader,const char* name,const Color& color) {
			D3DXHANDLE handle = findHandle(shader,name);
			if ( handle != 0 ) {
				shader.m_FX->SetValue(handle,&color,sizeof(Color));
				return true;
			}
			return false;
		}

		bool setFloat(Shader& shader,const char* name,float value) {
			D3DXHANDLE handle = findHandle(shader,name);
			if ( handle != 0 ) {
				shader.m_FX->SetFloat(handle,value);
				return true;
			}
			return false;
		}

		D3DXHANDLE findHandle(const Shader& shader,const char* name) {
			IdString hashName = string::murmur_hash(name);
			ShaderConstant* sh = shader.constants;
			for ( int i = 0; i < shader.constantCount; ++i ) {
				if ( hashName == sh->name ) {
					return sh->handle;
				}
				++sh;
			}				
			return 0;
		}

		int createTransformedTextureColorShader(Renderer* renderer,int textureId) {
			const char* g_strBuffer = 
				"texture gTex;\r\n"
				"sampler TexS = sampler_state {\r\n"
				"Texture = <gTex>;\r\n"
				"MinFilter = LINEAR;\r\n"
				"MagFilter = LINEAR;\r\n"
				"MipFilter = LINEAR;\r\n"
				"AddressU  = WRAP;\r\n"
				"AddressV  = WRAP;\r\n"
				"};\r\n"
				"struct OutputVS {\r\n"
				"float4 posH   : POSITION0;\r\n"
				"float2 tex0   : TEXCOORD0;\r\n"
				"float4 color  : COLOR0;\r\n"
				"};\r\n"
				"OutputVS TTCVVS(float4 posL : POSITION0, float2 tex0: TEXCOORD0,float4 color0 : COLOR0) {\r\n"
				"OutputVS outVS = (OutputVS)0;\r\n"
				"outVS.posH = posL;\r\n"
				"outVS.tex0 = tex0; \r\n"
				"outVS.color = color0;\r\n"
				"return outVS;\r\n"
				"}\r\n"
				"float4 TTCVPS(float2 texCoord : TEXCOORD0,float4 color : COLOR0) : COLOR {\r\n"
				"float4 t = tex2D(TexS, texCoord);\r\n"
				"return color * t;     \r\n"
				"}\r\n"
				"technique TTCVTech {\r\n"
				"pass P0 {\r\n"
				"vertexShader = compile vs_2_0 TTCVVS();\r\n"
				"pixelShader  = compile ps_2_0 TTCVPS();\r\n"	
				"}\r\n"
				"}\r\n";
			int ret = renderer->createShaderFromText(g_strBuffer,"TTCVTech");
			Shader sh = renderer->getShader(ret);
			shader::setTexture(sh,"gTex",renderer,textureId);
			return ret;
		}
	}

	
}