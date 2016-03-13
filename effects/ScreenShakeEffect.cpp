#include "ScreenShakeEffect.h"

namespace ds {

	// ------------------------------------------
	// gray fade effect
	// ------------------------------------------
	ScreenShakeEffect::ScreenShakeEffect() {
		_rtID = renderer::createRenderTarget(Color(0, 0, 0, 0));
		_shader = createShader();
		_settings.load();
	}

	ScreenShakeEffect::~ScreenShakeEffect() {

	}

	void ScreenShakeEffect::start() {
		if (isActive()) {
			renderer::setRenderTarget(_rtID);
		}
	}

	void ScreenShakeEffect::onActivation() {
		_timer = 0.0f;
	}

	void ScreenShakeEffect::tick(float dt) {
		if (isActive()) {
			_timer += dt;
			if (_timer >= _settings.ttl) {
				deactivate();
			}
		}
	}

	void ScreenShakeEffect::render() {
		if (isActive()) {
			// restore backbuffer
			renderer::restoreBackBuffer();			
			// draw rendertarget
			float n = _timer / _settings.ttl;
			n = math::clamp(n, 0.0f, 1.0f);
			_shader->setFloat("timer", n * PI);
			_shader->setFloat("frequency", _settings.frequency);
			float amp = _settings.amplitude / renderer::getScreenWidth();
			_shader->setFloat("amplitude", amp);
			renderer::draw_render_target(_rtID, _shader->getID());
		}
	}

	Shader* ScreenShakeEffect::createShader() {
		const char* g_strBuffer =
			"uniform extern float4x4 gWVP;\r\n"
			"uniform extern texture gTex;\r\n"
			"float timer = 1.0f; \r\n"
			"float frequency = 1.0f; \r\n"
			"float amplitude = 1.0f; \r\n"
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
			"OutputVS ShakeVS(float3 posL : POSITION0,float2 tex0 : TEXCOORD0 , float4 color : COLOR0) {\r\n"
			"	OutputVS outVS = (OutputVS)0;	\r\n"
			"	outVS.posH = mul(float4(posL, 1.0f), gWVP);		\r\n"
			"	outVS.posH.x += cos(3.14 * timer * frequency) * amplitude;\r\n"
			"	outVS.tex0 = tex0;\r\n"
			"	outVS.color0 = color;\r\n"
			"	return outVS;\r\n"
			"}\r\n"
			"float4 ShakePS(OutputVS input) : COLOR {\r\n"
			"	return tex2D(TexS, input.tex0);\r\n"
			"}\r\n"
			"technique ShakeTech {\r\n"
			"	pass P0 {\r\n"
			"		vertexShader = compile vs_2_0 ShakeVS();\r\n"
			"		pixelShader  = compile ps_2_0 ShakePS();\r\n"
			"	}\r\n"
			"}\r\n";
		int ret = ds::renderer::createShaderFromText(g_strBuffer, "ShakeTech");
		Shader* s = ds::renderer::getShader(ret);
		return s;
	}

}
