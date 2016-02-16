#include "shader.h"
#include "..\utils\StringUtils.h"
#include "graphics.h"
#include "..\utils\Log.h"

#ifdef DEBUG
DWORD SHADER_FLAGS = D3DXFX_NOT_CLONEABLE | D3DXSHADER_DEBUG;
#else
DWORD SHADER_FLAGS = D3DXFX_NOT_CLONEABLE;
#endif

namespace ds {

	Shader::Shader(int id,const char* name) : _id(id) , _hashName(0), _FX(0), _constants(0), _constantCount(0) {
		_hashName = string::murmur_hash(name);
		sprintf_s(debugName, 32, "%s", name);
	}

	bool Shader::setFloat(const char* name, float value) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetFloat(handle, value);
			return true;
		}
		return false;
	}

	bool Shader::setVector3f(const char* name, const Vector3f& v) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetValue(handle, &v, sizeof(Vector3f));
			return true;
		}
		return false;
	}

	bool Shader::setVector2f(const char* name, const Vector2f& v) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetValue(handle, &v, sizeof(Vector2f));
			return true;
		}
		return false;
	}

	bool Shader::setValue(const char* name, void* data, UINT size) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetValue(handle, &data, size);
			return true;
		}
		return false;
	}

	bool Shader::setTexture(const char* name, int textureID) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetTexture(handle, renderer::getDirectTexture(textureID));
			return true;
		}
		return false;
	}

	bool Shader::setMatrix(const char* name, const mat4& m) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetValue(handle, m,sizeof(mat4));
			return true;
		}
		return false;
	}

	bool Shader::setColor(const char* name, const Color& color) {
		D3DXHANDLE handle = findHandle(name);
		if (handle != 0) {
			_FX->SetValue(handle, &color, sizeof(Color));
			return true;
		}
		return false;
	}

	bool Shader::contains(const char* name) {
		return findHandle(name) != 0;
	}

	D3DXHANDLE Shader::findHandle(const char* name) {
		IdString hashName = string::murmur_hash(name);
		ShaderConstant* sh = _constants;
		for (uint32 i = 0; i < _constantCount; ++i) {
			if (hashName == sh->name) {
				return sh->handle;
			}
			++sh;
		}
		return 0;
	}

	void Shader::initialize(const char* techName) {
		LOG << "initializing shader using tech: " << techName;
		_hTech = _FX->GetTechniqueByName(techName);
		D3DXEFFECT_DESC effectDesc;
		_FX->GetDesc(&effectDesc);
		UINT nc = effectDesc.Parameters;
		_constants = new ShaderConstant[nc];
		_constantCount = nc;
		LOG << "Got Description - number of parameters: " << nc;
		for (UINT i = 0; i < effectDesc.Parameters; ++i) {
			D3DXHANDLE hParam = _FX->GetParameter(NULL, i);
			D3DXPARAMETER_DESC pDesc;
			// get parameter description
			_FX->GetParameterDesc(hParam, &pDesc);
			LOG << "Parameter : " << pDesc.Name << " Type: " << pDesc.Type;
			_constants[i].handle = hParam;
			_constants[i].name = string::murmur_hash(pDesc.Name);
		}
		LOG << "Shader finally loaded";

	}

	uint32 Shader::start() {
		HR(_FX->SetTechnique(_hTech));
		UINT numPasses = 0;
		HR(_FX->Begin(&numPasses, 0));
		return numPasses;
	}

	void Shader::end() {
		HR(_FX->End());
	}

	void Shader::release() {
		delete[] _constants;
		SAFE_RELEASE(_FX);
	}

	void Shader::createFromText(const char* buffer, const char* techName) {
		// FIXME: do not use techName
		_hashName = string::murmur_hash(techName);
		UINT dwBufferSize = (UINT)strlen(buffer) + 1;
		ID3DXBuffer* errors = 0;
		D3DXCreateEffect(renderer::getDevice(), buffer, dwBufferSize, 0, 0, SHADER_FLAGS, 0, &_FX, &errors);
		if (errors != 0) {
			LOGE << "Error while loading shader: " << (char*)errors->GetBufferPointer();
		}
		else {
			LOG << "Shader created";
			initialize(techName);
		}
	}

	bool Shader::loadShader(const char* fxName, const char* techName) {
		char fileName[256];
		sprintf(fileName, "content\\effects\\%s.fx", fxName);
		ID3DXBuffer* errors = 0;
		HRESULT hr = D3DXCreateEffectFromFileA(renderer::getDevice(), fileName, 0, 0, SHADER_FLAGS, 0, &_FX, &errors);
		if (errors != 0) {
			LOGE << "Error while loading shader: " << (char*)errors->GetBufferPointer();
			return false;
		}

		LOG << "Shader created";
		initialize(techName);
		return true;
	}

	void Shader::beginPass(UINT p) {
		HR(_FX->BeginPass(p));
	}

	void Shader::endPass() {
		HR(_FX->EndPass());
	}

	void Shader::commitChanges() {
		_FX->CommitChanges();
	}

	//void Shader::load(BinaryLoader* loader) {
		// FIXME: clean up before
		/*
		while (loader->openChunk() == 0) {
			if (loader->getChunkID() == CHNK_SPRITE) {
				std::string name;
				std::string tech;
				std::string code;
				loader->read(name);
				loader->read(tech);
				loader->read(code);	
				if (_FX != 0) {
					LOG << "reloading shader - gotta clean up first";
					release();
				}
				createFromText(code.c_str(), tech.c_str());
			}
			loader->closeChunk();
		}
		*/
	//}

	namespace shader {

		int createTransformedTextureColorShader(int textureId) {
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
			Shader* sh = renderer::getShader(ret);
			sh->setTexture("gTex",textureId);
			return ret;
		}


		int createParticleShader() {
			const char* g_strBuffer =
				"uniform extern float4x4 gWVP;\r\n"
				"uniform extern texture gTex; \r\n"
				"uniform extern float2 viewportPosition = float2(512, 384); \r\n"
				"float VP_ARRAY[8] = { -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f }; \r\n"
				"sampler TexS = sampler_state{ \r\n"
				"Texture = <gTex>; \r\n"
				"MinFilter = LINEAR; \r\n"
				"MagFilter = LINEAR; \r\n"
				"MipFilter = LINEAR; \r\n"
				"AddressU = CLAMP; \r\n"
				"AddressV = CLAMP; \r\n"
				"}; \r\n"
				"struct OutputVS {\r\n"
				"float4 posH : POSITION0; \r\n"
				"float2 tex0 : TEXCOORD0; \r\n"
				"float4 color0 : COLOR0; \r\n"
				"}; \r\n"
				"OutputVS BasicVS(float3 posL : POSITION0, float2 tex0 : TEXCOORD0, float2 scale : TEXCOORD1, float2 dim : TEXCOORD2, float2 rotindex : TEXCOORD3, float4 color : COLOR0) {\r\n"
				"OutputVS outVS = (OutputVS)0; \r\n"
				"outVS.tex0 = tex0; \r\n"
				"outVS.color0 = color; \r\n"
				"float3 pos = posL; \r\n"
				"pos -= float3(viewportPosition, 0.0f); \r\n"
				"float px = VP_ARRAY[rotindex.y * 2] * dim.x; \r\n"
				"float py = VP_ARRAY[rotindex.y * 2 + 1] * dim.y; \r\n"
				"float sx = px * scale.x; \r\n"
				"float sy = py * scale.y; \r\n"
				"float xt = cos(rotindex.x) * sx - sin(rotindex.x) * sy; \r\n"
				"float yt = sin(rotindex.x) * sx + cos(rotindex.x) * sy; \r\n"
				"xt += pos.x; \r\n"
				"yt += pos.y; \r\n"
				"outVS.posH = mul(float4(xt, yt, 0.0f, 1.0f), gWVP); \r\n"
				"return outVS; \r\n"
				"}\r\n"
				"float4 BasicPS(OutputVS input) : COLOR{ \r\n"
				"float4 clr = tex2D(TexS, input.tex0); \r\n"
				"return clr * input.color0; \r\n"
				"}\r\n"
				"technique ParticleTech{ \r\n"
				"pass P0{ \r\n"
				"vertexShader = compile vs_2_0 BasicVS(); \r\n"
				"pixelShader = compile ps_2_0 BasicPS(); \r\n"
				"}\r\n"
				"}; \r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "ParticleTech");
			return ret;
		}
		// -------------------------------------------------------
		// Create a position texture color shader
		// -------------------------------------------------------
		int createPTCShader(int textureId) {
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
			Shader* sh = renderer::getShader(ret);
			if ( textureId != -1 ) {
				sh->setTexture("gTex",textureId);
			}
			return ret;
		}

		int createBloomShader(int textureID,float threshold) {
			const char* g_strBuffer = 
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"uniform extern float Threshold = 0.25;\r\n"
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
			"OutputVS BloomVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
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
			"		vertexShader = compile vs_2_0 BloomVS();\r\n"
			"		pixelShader = compile ps_2_0 BloomPS();\r\n"
			"	}\r\n"
			"}\r\n";
			int ret = renderer::createShaderFromText(g_strBuffer, "BloomTech");
			Shader* sh = renderer::getShader(ret);
			sh->setTexture("gTex",textureID);
			sh->setFloat("Threshold",threshold);
			return ret;
		}

		int createBlurShader(int textureID) {
			const char* g_strBuffer =
				"uniform extern float4x4 gWVP;\r\n"
				"uniform extern texture gTex;\r\n"
				"#define SAMPLE_COUNT 15\r\n"
				"uniform extern float2 SampleOffsets[SAMPLE_COUNT];\r\n"
				//"uniform extern float SX[SAMPLE_COUNT] = {-2.5,-1.5,0,1.5,2.5,-2.5,-1.5,0,1.5,2.5,-2.5,-1.5,0,1.5,2.5};\r\n"
				//"uniform extern float SY[SAMPLE_COUNT] = {1.5,1.5,1.5,1.5,1.5,0,0,0,0,0,-1.5,-1.5,-1.5,-1.5,-1.5};\r\n"
				"uniform extern float SampleWeights[SAMPLE_COUNT] = {0.106115,0.102851,0.102851,0.0936465,0.0936465,0.0801001,0.0801001,0.0643622,0.0643622,0.0485832,0.0485832,0.0344506,0.0344506,0.0229491,0.0229491};\r\n"
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
			Shader* sh = renderer::getShader(ret);
			sh->setTexture("gTex",textureID);
			return ret;
		}

		int createBloomCombineShader(int colorTextureID,int bloomTextureID) {
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
			"	float grey = dot(color.rgb, float3(0.3, 0.59, 0.11));  \r\n"
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
			Shader* sh = renderer::getShader(ret);
			sh->setTexture("gTex",bloomTextureID);
			sh->setTexture("ColorMap",colorTextureID);
			return ret;
		}

		int createCombineLightShader(int lightTextureID, int colorTextureID) {
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
			Shader* sh = renderer::getShader(ret);
			sh->setTexture("LightMap",  lightTextureID);
			sh->setTexture("ColorMap",  colorTextureID);
			return ret;
		}

		Shader* createFadeToGrayShader(int textureID) {
			const char* g_strBuffer =
				"uniform extern float4x4 gWVP;\r\n"
				"uniform extern texture gTex;\r\n"
				"float timer = 1.0f; \r\n"
				"float4 grayscale = float4(0.2125, 0.7154, 0.0721, 1.0); \r\n"
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
				"	float3 greyscale = dot(clr.rgb, float3(0.30, 0.59, 0.11)); \r\n"
				"	float3 rgb = lerp(clr.rgb, greyscale, timer); \r\n"
				"	return float4(rgb, clr.a);\r\n"
				"}\r\n"
				"technique FadeTech {\r\n"
				"	pass P0 {\r\n"
				"		vertexShader = compile vs_2_0 BasicVS();\r\n"
				"		pixelShader  = compile ps_2_0 BasicPS();\r\n"
				"	}\r\n"
				"}\r\n";
			int ret = ds::renderer::createShaderFromText(g_strBuffer, "FadeTech");
			Shader* s = ds::renderer::getShader(ret);
			if (textureID != -1) {
				s->setTexture("gTex", textureID);
			}
			return s;
		}


	}

	
}