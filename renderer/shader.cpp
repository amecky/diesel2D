#include "shader.h"
#include "..\utils\StringUtils.h"
#include "assets.h"

namespace ds {

	namespace shader {

		bool setVector3f(Shader& shader,const char* name,const Vector3f& v) {
			D3DXHANDLE handle = findHandle(shader,name);
			if ( handle != 0 ) {
				shader.m_FX->SetValue(handle,&v,sizeof(Vector3f));
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
				shader.m_FX->SetTexture(handle,assets::getDirectTexture(textureID));
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
			for ( uint32 i = 0; i < shader.constantCount; ++i ) {
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
				"AddressU  = CLAMP;\r\n"
				"AddressV  = CLAMP;\r\n"
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
			int ret = renderer::createShaderFromText(g_strBuffer, "TTCVTech");
			Shader sh = renderer::getShader(ret);
			shader::setTexture(sh,"gTex",renderer,textureId);
			return ret;
		}

		// -------------------------------------------------------
		// Create a position texture color shader
		// -------------------------------------------------------
		int createPTCShader(Renderer* renderer,int textureId) {
			const char* g_strBuffer = 
				"uniform extern float4x4 gWVP;\r\n"
				"uniform extern texture gTex;\r\n"
				"sampler TexS = sampler_state {\r\n"
				"	Texture = <gTex>;\r\n"
				"	MinFilter = LINEAR;\r\n"
				"	MagFilter = LINEAR;\r\n"
				"	MipFilter = LINEAR;\r\n"
				"	AddressU  = CLAMP;\r\n"
				"	AddressV  = CLAMP;\r\n"
				"};\r\n"
				"struct OutputVS {\r\n"
				"	float4 posH   : POSITION0;\r\n"
				"	float2 tex0   : TEXCOORD0;\r\n"
				"	float4 color0 : COLOR0;\r\n"
				"};\r\n"
				"OutputVS BasicVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
				"	OutputVS outVS = (OutputVS)0;	\r\n"
				"	outVS.posH = mul(float4(posL, 1.0f), gWVP);		\r\n"
				"	outVS.tex0 = tex0;\r\n"
				"	outVS.color0 = color;\r\n"
				"	return outVS;\r\n"
				"}\r\n"
				"float4 BasicPS(OutputVS input) : COLOR {\r\n"
				"	float4 clr = tex2D(TexS, input.tex0);\r\n"
				"	return clr * input.color0;\r\n"
				"}\r\n"
				"technique PTCTech {\r\n"
				"	pass P0 {\r\n"
				"		vertexShader = compile vs_2_0 BasicVS();\r\n"
				"		pixelShader  = compile ps_2_0 BasicPS();\r\n"
				"	}\r\n"
				"}\r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "PTCTech");
			Shader sh = renderer::getShader(ret);
			if ( textureId != -1 ) {
				shader::setTexture(sh,"gTex",renderer,textureId);
			}
			return ret;
		}

		int createBloomShader(Renderer* renderer,int textureID,float threshold) {
			const char* g_strBuffer = 
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"float Threshold = 0.25;\r\n"
			"sampler TexS = sampler_state {\r\n"
			"	Texture = <gTex>;\r\n"
			"	MinFilter = LINEAR;\r\n"
			"	MagFilter = LINEAR;\r\n"
			"	MipFilter = LINEAR;\r\n"
			"	//AddressU  = CLAMP;\r\n"
			"	//AddressV  = CLAMP;\r\n"
			"};\r\n"
			"struct OutputVS {\r\n"
			"	float4 posH   : POSITION0;\r\n"
			"	float2 Tex   : TEXCOORD0;\r\n"
			"	float4 color0 : COLOR0;\r\n"
			"};\r\n"
			"OutputVS FadeVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
			"	OutputVS outVS = (OutputVS)0;	\r\n"
			"	outVS.posH = mul(float4(posL, 1.0f), gWVP);		\r\n"
			"	outVS.Tex = tex0;\r\n"
			"	outVS.color0 = color;\r\n"
			"	return outVS;\r\n"
			"}\r\n"
			"float4 BloomPS(OutputVS input) : COLOR0 {\r\n"
			"	float4 Color = tex2D(TexS, input.Tex);\r\n"
			"	return saturate((Color - Threshold) / (1 - Threshold));\r\n"
			"}\r\n"
			"technique BloomTech {\r\n"
			"	pass P0 {\r\n"
			"		vertexShader = compile vs_2_0 FadeVS();\r\n"
			"		pixelShader = compile ps_2_0 BloomPS();\r\n"
			"	}\r\n"
			"}\r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "BloomTech");
			Shader sh = renderer::getShader(ret);
			shader::setTexture(sh,"gTex",renderer,textureID);
			shader::setFloat(sh,"Threshold",threshold);
			return ret;
		}

		int createBlurShader(Renderer* renderer,int textureID) {
			const char* g_strBuffer =
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"#define SAMPLE_COUNT 15\r\n"
			"uniform extern float2 SampleOffsets[SAMPLE_COUNT];\r\n"
			"uniform extern float SampleWeights[SAMPLE_COUNT];// = {0.106115,0.102851,0.102851,0.0936465,0.0936465,0.0801001,0.0801001,0.0643622,0.0643622,0.0485832,0.0485832,0.0344506,0.0344506,0.0229491,0.0229491};\r\n"
			"sampler TexS = sampler_state {\r\n"
			"	Texture = <gTex>;\r\n"
			"	MinFilter = LINEAR;\r\n"
			"	MagFilter = LINEAR;\r\n"
			"	MipFilter = LINEAR;\r\n"
			"	AddressU  = CLAMP;\r\n"
			"	AddressV  = CLAMP;\r\n"
			"};\r\n"
			"struct OutputVS {\r\n"
			"	float4 posH   : POSITION0;\r\n"
			"	float2 Tex   : TEXCOORD0;\r\n"
			"	float4 color0 : COLOR0;\r\n"
			"};\r\n"
			"OutputVS BlurVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
			"	OutputVS outVS = (OutputVS)0;	\r\n"
			"	outVS.posH = mul(float4(posL, 1.0f), gWVP);		\r\n"
			"	outVS.Tex = tex0;\r\n"
			"	outVS.color0 = color;\r\n"
			"	return outVS;\r\n"
			"}\r\n"
			"float4 BlurPS(OutputVS input) : COLOR {\r\n"
			"	float4 c = 0;    \r\n"
			"	for (int i = 0; i < SAMPLE_COUNT; i++) {\r\n"
			"		c += tex2D(TexS, input.Tex + SampleOffsets[i]) * SampleWeights[i];\r\n"
			"	}    \r\n"
			"	return c;\r\n"
			"}\r\n"
			"technique BlurTech {\r\n"
			"	pass P0 {\r\n"
			"		vertexShader = compile vs_2_0 BlurVS();\r\n"
			"		pixelShader = compile ps_2_0 BlurPS();\r\n"
			"	}\r\n"
			"}\r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "BlurTech");
			Shader sh = renderer::getShader(ret);
			shader::setTexture(sh,"gTex",renderer,textureID);
			return ret;
		}

		int createBloomCombineShader(Renderer* renderer,int colorTextureID,int bloomTextureID) {
			const char* g_strBuffer =
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"uniform extern texture ColorMap;\r\n"
			"uniform extern texture gBlurTex;\r\n"
			"sampler TexS = sampler_state {\r\n"
			"	Texture = <gTex>;\r\n"
			"	MinFilter = LINEAR;\r\n"
			"	MagFilter = LINEAR;\r\n"
			"	MipFilter = LINEAR;\r\n"
			"	AddressU  = Clamp;\r\n"
			"	AddressV  = Clamp;\r\n"
			"};\r\n"
			"sampler ColorMapSampler = sampler_state {\r\n"
			"	Texture = <ColorMap>;\r\n"
			"	MinFilter = Linear;\r\n"
			"	MagFilter = Linear;\r\n"
			"	MipFilter = Linear;\r\n"
			"	AddressU  = Clamp;\r\n"
			"	AddressV  = Clamp;\r\n"
			"};\r\n"
			"float BloomIntensity = 2.3;\r\n"
			"float OriginalIntensity = 1.0;\r\n"
			"float BloomSaturation = 1.0;\r\n"
			"float OriginalSaturation = 1.0;\r\n"
			"float4 AdjustSaturation(float4 color, float saturation) {\r\n"
			"	float grey = dot(color, float4(0.3, 0.59, 0.11,1.0));  \r\n"
			"	return lerp(grey, color, saturation);\r\n"
			"}\r\n"
			"struct OutputVS {\r\n"
			"	float4 posH   : POSITION0;\r\n"
			"	float2 Tex   : TEXCOORD0;\r\n"
			"	float4 color0 : COLOR0;\r\n"
			"};\r\n"
			"OutputVS FadeVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
			"	OutputVS outVS = (OutputVS)0; \r\n"
			"	outVS.posH = mul(float4(posL, 1.0f), gWVP);   \r\n"
			"	outVS.Tex = tex0;\r\n"
			"	outVS.color0 = color;\r\n"
			"	return outVS;\r\n"
			"}\r\n"
			"float4 BCPS(OutputVS input) : COLOR0 {\r\n"
			"	float4 bloomColor = tex2D(TexS, input.Tex);\r\n"
			"	float4 originalColor = tex2D(ColorMapSampler, input.Tex);\r\n"
			"	bloomColor = AdjustSaturation(bloomColor, BloomSaturation) * BloomIntensity;\r\n"
			"	originalColor = AdjustSaturation(originalColor, OriginalSaturation) * OriginalIntensity;\r\n"
			"	originalColor *= (1 - saturate(bloomColor));\r\n"
			"	return originalColor + bloomColor;\r\n"
			"}\r\n"
			"technique BCTech {\r\n"
			"	pass P0 {\r\n"
			"		vertexShader = compile vs_2_0 FadeVS();\r\n"
			"		pixelShader = compile ps_2_0 BCPS();\r\n"
			"	}\r\n"
			"}\r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "BCTech");
			Shader sh = renderer::getShader(ret);
			shader::setTexture(sh,"gTex",renderer,bloomTextureID);
			shader::setTexture(sh,"ColorMap",renderer,colorTextureID);
			return ret;
		}

		int createCombineLightShader(Renderer* renderer, int lightTextureID, int colorTextureID) {
			const char* g_strBuffer =
				"uniform extern float4x4 gWVP;\r\n"
				"uniform extern texture LightMap;\r\n"
				"uniform extern texture ColorMap;\r\n"
				"float4 ambientLight = float4(0.1,0.1,0.1,1.0);\r\n"
				"sampler LightMapSampler = sampler_state {\r\n"
				"	Texture = <LightMap>;\r\n"
				"	MinFilter = LINEAR;\r\n"
				"	MagFilter = LINEAR;\r\n"
				"	MipFilter = LINEAR;\r\n"
				"	AddressU  = Clamp;\r\n"
				"	AddressV  = Clamp;\r\n"
				"};\r\n"
				"sampler ColorMapSampler = sampler_state {\r\n"
				"	Texture = <ColorMap>;\r\n"
				"	MinFilter = Linear;\r\n"
				"	MagFilter = Linear;\r\n"
				"	MipFilter = Linear;\r\n"
				"	AddressU  = Clamp;\r\n"
				"	AddressV  = Clamp;\r\n"
				"};\r\n"
				"struct OutputVS {\r\n"
				"	float4 posH   : POSITION0;\r\n"
				"	float2 Tex   : TEXCOORD0;\r\n"
				"	float4 color0 : COLOR0;\r\n"
				"};\r\n"
				"OutputVS FadeVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
				"	OutputVS outVS = (OutputVS)0; \r\n"
				"	outVS.posH = mul(float4(posL, 1.0f), gWVP);   \r\n"
				"	outVS.Tex = tex0;\r\n"
				"	outVS.color0 = color;\r\n"
				"	return outVS;\r\n"
				"}\r\n"
				"float4 BCPS(OutputVS input) : COLOR0 {\r\n"
				"	float4 lightColor = tex2D(LightMapSampler, input.Tex);\r\n"
				"	float4 originalColor = tex2D(ColorMapSampler, input.Tex);\r\n"
				"   float4 lc = lightColor + ambientLight;\r\n"
				"	return originalColor * saturate(lc);\r\n"
				"}\r\n"
				"technique BCTech {\r\n"
				"	pass P0 {\r\n"
				"		vertexShader = compile vs_2_0 FadeVS();\r\n"
				"		pixelShader = compile ps_2_0 BCPS();\r\n"
				"	}\r\n"
				"}\r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "BCTech");
			Shader sh = renderer::getShader(ret);
			shader::setTexture(sh, "LightMap", renderer, lightTextureID);
			shader::setTexture(sh, "ColorMap", renderer, colorTextureID);
			return ret;
		}
	}

	
}