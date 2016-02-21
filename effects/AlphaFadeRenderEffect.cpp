#include "AlphaFadeRenderEffect.h"

namespace ds {

	// ------------------------------------------
	// gray fade effect
	// ------------------------------------------
	AlphaFadeEffect::AlphaFadeEffect() {
		// create render target
		_rtID = renderer::createRenderTarget(Color(0, 0, 0, 0));
		// load shader
		_shader = createShader();
		_settings.load();
	}

	AlphaFadeEffect::~AlphaFadeEffect() {

	}

	void AlphaFadeEffect::start() {
		if (isActive()) {
			// set render target
			renderer::setRenderTarget(_rtID);
		}
	}

	void AlphaFadeEffect::onActivation() {
		_timer = 0.0f;
	}

	void AlphaFadeEffect::tick(float dt) {
		if (isActive()) {
			_timer += dt;
			if (_timer >= _settings.ttl) {
				if (_settings.mode < 0) {
					_timer = 0.0f;
				}
				else if (_settings.mode == 0) {
					_timer = _settings.ttl;
				}
				else {
					--_settings.mode;
					_timer = 0.0f;

				}
			}
		}
	}

	void AlphaFadeEffect::render() {
		if (isActive()) {
			// restore backbuffer
			renderer::restoreBackBuffer();
			// draw rendertarget
			float n = _timer / _settings.ttl;
			n = math::clamp(n, 0.0f, 1.0f);
			tweening::TweeningType tweening = tweening::get_by_index(_settings.tweeningType);
			float v = tweening::interpolate(tweening, 0.0f, 1.0f, _timer, _settings.ttl);
			_shader->setFloat("timer", v);
			renderer::draw_render_target(_rtID, _shader->getID());
		}
	}

	Shader* AlphaFadeEffect::createShader() {
		const char* g_strBuffer =
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"float timer = 1.0f; \r\n"
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
			"	clr.a = clr.a * (1.0 - timer);\r\n"
			"	return clr;\r\n"
			"}\r\n"
			"technique AlphaFadeTech {\r\n"
			"	pass P0 {\r\n"
			"		vertexShader = compile vs_2_0 BasicVS();\r\n"
			"		pixelShader  = compile ps_2_0 BasicPS();\r\n"
			"	}\r\n"
			"}\r\n";
		int ret = ds::renderer::createShaderFromText(g_strBuffer, "AlphaFadeTech");
		Shader* s = ds::renderer::getShader(ret);
		return s;
	}

}
