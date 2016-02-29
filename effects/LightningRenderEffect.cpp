#include "LightningRenderEffect.h"
#include "..\sprites\SpriteBatch.h"

namespace ds {

	// ------------------------------------------
	// gray fade effect
	// ------------------------------------------
	LightningRenderEffect::LightningRenderEffect() {
		// create render target
		_rtID = renderer::createRenderTarget(Color(0, 0, 0, 0));
		_lightRT = renderer::createRenderTarget(Color(0, 0, 0, 0));
		// load shader
		//_shader = createShader();
		_shaderID = renderer::loadShader("lights", "LightningTech");
		_shader = renderer::getShader("lights");
	}

	LightningRenderEffect::~LightningRenderEffect() {

	}

	void LightningRenderEffect::start() {
		if (isActive()) {
			// set render target
			renderer::setRenderTarget(_rtID);
		}
	}

	int LightningRenderEffect::add(const v2& pos, float radius) {
		Light l;
		l.pos = pos;
		l.radius = radius;
		l.id = _ids++;
		_lights.push_back(l);
		return l.id;
	}

	void LightningRenderEffect::onActivation() {
		_timer = 0.0f;
	}

	void LightningRenderEffect::tick(float dt) {
		if (isActive()) {
			_timer += dt;
		}
	}

	void LightningRenderEffect::render() {
		if (isActive()) {
			_shader->setTexture("gTex", _rtID.textureID);
			renderer::setRenderTarget(_lightRT);
			sprites::setShaderID(_shaderID);
			// draw lights
			for (int i = 0; i < _lights.size(); ++i) {
				const Light& l = _lights[i];
				sprites::draw(l.pos, math::buildTexture(0, 0, 1024, 1024), 0.0f, l.radius / 1024.0f, l.radius / 1024.0f);
			}
			// restore backbuffer
			renderer::restoreBackBuffer();
			// draw rendertarget
			renderer::draw_render_target(_lightRT);// , _shader->getID());
		}
	}

	Shader* LightningRenderEffect::createShader() {
		const char* g_strBuffer =
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"float width = 0.7; \r\n"
			"float edge = 0.3; \r\n"
			"float4 color = float4(1.0,0.0,0.0,1.0); \r\n"
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
			"OutputVS LightningBasicVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
			"	OutputVS outVS = (OutputVS)0;	\r\n"
			"	outVS.posH = mul(float4(posL, 1.0f), gWVP);		\r\n"
			"	outVS.tex0 = tex0;\r\n"
			"	outVS.color0 = color;\r\n"
			"	return outVS;\r\n"
			"}\r\n"
			"float4 LightningBasicPS(OutputVS input) : COLOR {\r\n"
			"	float distance = 1.0 - tex2D(TexS, input.tex0).a;\r\n"
			"	float alpha = 1.0 - smoothstep(width,width + edge,distance);\r\n"
			"	float4 clr = color;//tex2D(TexS, input.tex0);\r\n"
			"	clr.a = alpha;\r\n"
			"	return clr;\r\n"
			"}\r\n"
			"technique LightningTech {\r\n"
			"	pass P0 {\r\n"
			"		vertexShader = compile vs_2_0 LightningBasicVS();\r\n"
			"		pixelShader  = compile ps_2_0 LightningBasicPS();\r\n"
			"	}\r\n"
			"}\r\n";
		int ret = ds::renderer::createShaderFromText(g_strBuffer, "LightningTech");
		Shader* s = ds::renderer::getShader(ret);
		return s;
	}

}
