#pragma once
#include "RenderEffect.h"

#include "..\renderer\render_types.h"
#include "..\sprites\Sprite.h"
#include "..\sprites\SpriteBatch.h"

namespace ds {

const int SAMPLE_COUNT = 15;

// -------------------------------------------------------
// Bloom settings
// -------------------------------------------------------
struct BloomSettings : public DynamicGameSettings {

	float threshold;
	float intensity;
	float originalIntensity;
	float saturation;
	float originalSaturation;
	float blurAmount;

	BloomSettings() {
		addFloat("threshold", &threshold, 0.4f);
		addFloat("intensity", &intensity, 1.0f);
		addFloat("original_intensity", &originalIntensity, 1.0f);
		addFloat("saturation", &saturation, 1.0f);
		addFloat("original_saturation", &originalSaturation, 1.0f);
		addFloat("blur_amount", &blurAmount, 1.0f);
	}

	const char* getFileName() const {
		return "bloom.json";
	}

};

// -------------------------------------------------------
// Bloom component
// -------------------------------------------------------
class BloomRenderEffect : public RenderEffect {

public:
	BloomRenderEffect(void);
	virtual ~BloomRenderEffect(void);


	void start();
	void render();

	DynamicGameSettings* getSettings() {
		return &_settings;
	}
private:
	float computeGaussian(float n);
	void createBlurParameters(float dx, float dy, v2* sampleOffsets, float* weights, int sampleCount);
	BloomSettings _settings;
	RTID _baseRT;
	RTID _bloomRT;
	RTID _blurRT;
	RTID _blurNextRT;
	RTID _bloomCombineRT;

	Shader* m_BlurHShader;
	Shader* m_BlurVShader;
	Shader* m_BloomShader;
	Shader* m_BloomCombineShader;
	int m_BloomShaderID;
	int m_BlurHShaderID;
	int m_BloomCombineShaderID;

	v2 _verticalOffset[15];
	float _verticalWeights[15];

	v2 _horizonzalOffset[15];
	float _horizonzalWeights[15];

};


}
