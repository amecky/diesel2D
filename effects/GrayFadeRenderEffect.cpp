#include "GrayFadeRenderEffect.h"

namespace ds {

	// ------------------------------------------
	// gray fade effect
	// ------------------------------------------
	GrayFadeEffect::GrayFadeEffect() {
		// create render target
		_rtID = renderer::createRenderTarget(Color(0, 0, 0, 0));
		// load shader
		_shader = createShader();
		_settings.load();
	}

	GrayFadeEffect::~GrayFadeEffect() {

	}

	void GrayFadeEffect::start() {
		if (isActive()) {
			// set render target
			renderer::setRenderTarget(_rtID);
		}
	}

	void GrayFadeEffect::onActivation() {
		_timer = 0.0f;
	}

	void GrayFadeEffect::tick(float dt) {
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

	void GrayFadeEffect::render() {
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

	Shader* GrayFadeEffect::createShader() {
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
		return s;
	}

}
