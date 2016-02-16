#include "BloomRenderEffect.h"
#include "..\renderer\shader.h"
#include "..\utils\Log.h"
#include "..\renderer\graphics.h"

namespace ds {

BloomRenderEffect::BloomRenderEffect() : RenderEffect() {
	Color clr(32, 32, 32, 0);
	_baseRT = renderer::createRenderTarget(clr);
	_bloomRT = renderer::createRenderTarget(clr);
	m_BloomShaderID = shader::createBloomShader(1, _settings.threshold);
	m_BloomShader = renderer::getShader(m_BloomShaderID);
	_blurRT = renderer::createRenderTarget(clr);
	_blurNextRT = renderer::createRenderTarget(clr);
	m_BlurHShaderID = shader::createBlurShader(_bloomRT.textureID);
	m_BlurHShader = renderer::getShader(m_BlurHShaderID);

	m_BlurHShader->setTexture("gTex", _bloomRT.textureID);
	m_BlurHShader->setFloat("BlurDistance", 1.0f / 1024.0f);
	// Bloom combine
	_bloomCombineRT = renderer::createRenderTarget(clr);
	m_BloomCombineShaderID = shader::createBloomCombineShader(_baseRT.textureID, _blurNextRT.textureID);
	m_BloomCombineShader = renderer::getShader(m_BloomCombineShaderID);
	_settings.load();

	createBlurParameters(1.0f / 1024.0f, 0.0f, _verticalOffset, _verticalWeights, 15);
	createBlurParameters(0.0f, 1.0f / 768.0, _horizonzalOffset, _horizonzalWeights, 15);
}


BloomRenderEffect::~BloomRenderEffect(void) {
}

void BloomRenderEffect::start() {
	// set render target
	if (isActive()) {
		renderer::setRenderTarget(_baseRT);
	}
}

void BloomRenderEffect::render() {
	if (isActive()) {
		// first step -> base RT using bloom shader
		//renderer::setRenderTarget(_bloomRT);
		//m_BloomShader->setFloat("Threshold", _settings.threshold);
		//renderer::draw_render_target(_baseRT, m_BloomShaderID);
		// second step -> blur 
		renderer::setRenderTarget(_blurRT);
		m_BlurHShader->setValue("SampleOffsets", _verticalOffset, sizeof(v2) * 15);
		//m_BlurHShader->setValue("SampleWeights", _verticalWeights, sizeof(float) * 15);
		renderer::draw_render_target(_baseRT, m_BlurHShaderID);
		// third step -> blur
		/*
		renderer::setRenderTarget(_blurNextRT);
		m_BlurHShader->setValue("SampleOffsets", _horizonzalOffset, sizeof(v2) * 15);
		m_BlurHShader->setValue("SampleWeights", _horizonzalWeights, sizeof(float) * 15);
		renderer::draw_render_target(_blurRT, m_BlurHShaderID);
		// fourth step -> combine original and this one		
		m_BloomCombineShader->setFloat("BloomIntensity", _settings.intensity);
		m_BloomCombineShader->setFloat("OriginalIntensity", _settings.originalIntensity);
		m_BloomCombineShader->setFloat("BloomSaturation", _settings.saturation);
		m_BloomCombineShader->setFloat("OriginalSaturation", _settings.originalSaturation);
		renderer::setRenderTarget(_bloomCombineRT);
		renderer::draw_render_target(_baseRT, m_BloomCombineShaderID);
		*/
		renderer::restoreBackBuffer();
		renderer::draw_render_target(_blurRT);
	}
}

void BloomRenderEffect::createBlurParameters(float dx, float dy, v2* sampleOffsets, float* weights, int sampleCount) {
	// The first sample always has a zero offset.
	weights[0] = computeGaussian(0);
	sampleOffsets[0] = v2(0,0);

	// Maintain a sum of all the weighting values.
	float totalWeights = weights[0];

	// Add pairs of additional sample taps, positioned
	// along a line in both directions from the center.
	for (int i = 0; i < sampleCount / 2; i++) {
		// Store weights for the positive and negative taps.
		float weight = computeGaussian(i + 1);

		weights[i * 2 + 1] = weight;
		weights[i * 2 + 2] = weight;

		totalWeights += weight * 2;

		// To get the maximum amount of blurring from a limited number of
		// pixel shader samples, we take advantage of the bilinear filtering
		// hardware inside the texture fetch unit. If we position our texture
		// coordinates exactly halfway between two texels, the filtering unit
		// will average them for us, giving two samples for the price of one.
		// This allows us to step in units of two texels per sample, rather
		// than just one at a time. The 1.5 offset kicks things off by
		// positioning us nicely in between two texels.
		float sampleOffset = i * 2 + 1.5f;

		v2 delta = v2(dx, dy) * sampleOffset;

		// Store texture coordinate offsets for the positive and negative taps.
		sampleOffsets[i * 2 + 1] = delta;
		sampleOffsets[i * 2 + 2] = -delta;
	}

	// Normalize the list of sample weightings, so they will always sum to one.
	for (int i = 0; i < 15; i++)
	{
		weights[i] /= totalWeights;
	}
}

float BloomRenderEffect::computeGaussian(float n) {
	float theta = _settings.blurAmount;
	return (float)((1.0 / sqrt(2 * PI * theta)) * exp(-(n * n) / (2.0 * theta * theta)));
}

}