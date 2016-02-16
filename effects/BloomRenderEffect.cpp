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
		renderer::setRenderTarget(_bloomRT);
		renderer::draw_render_target(_baseRT, m_BloomShaderID);
		// second step -> blur 
		renderer::setRenderTarget(_blurRT);
		renderer::draw_render_target(_bloomRT, m_BlurHShaderID);
		// third step -> blur
		renderer::setRenderTarget(_blurNextRT);
		renderer::draw_render_target(_blurRT, m_BlurHShaderID);
		// fourth step -> combine original and this one
		m_BloomShader->setFloat("Threshold", _settings.threshold);
		m_BloomCombineShader->setFloat("BloomIntensity", _settings.intensity);
		m_BloomCombineShader->setFloat("OriginalIntensity", _settings.originalIntensity);
		m_BloomCombineShader->setFloat("BloomSaturation", _settings.saturation);
		m_BloomCombineShader->setFloat("OriginalSaturation", _settings.originalSaturation);
		renderer::setRenderTarget(_bloomCombineRT);
		renderer::draw_render_target(_baseRT, m_BloomCombineShaderID);
		renderer::restoreBackBuffer();
		renderer::draw_render_target(_bloomCombineRT);
	}
}

}