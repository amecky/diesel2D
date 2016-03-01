#include "BlurRenderEffect.h"
#include "..\renderer\shader.h"
#include "..\utils\Log.h"
#include "..\renderer\graphics.h"

namespace ds {

BlurRenderEffect::BlurRenderEffect() : RenderEffect() {
	Color clr(32, 32, 32, 0);
	_baseRT = renderer::createRenderTarget(clr);
	_tempRT = renderer::createRenderTarget(640,360,clr);
	_blurVShaderID = renderer::loadShader("vblur", "VBlurTech");
	_blurHShaderID = renderer::loadShader("hblur", "HBlurTech");
}


BlurRenderEffect::~BlurRenderEffect(void) {
}

void BlurRenderEffect::start() {
	// set render target
	if (isActive()) {
		renderer::setRenderTarget(_baseRT);
	}
}

void BlurRenderEffect::render() {
	if (isActive()) {
		//renderer::restoreBackBuffer();
		renderer::setRenderTarget(_tempRT);
		renderer::draw_render_target(_baseRT, _blurVShaderID);
		//renderer::setRenderTarget(_baseRT);
		//renderer::draw_render_target(_tempRT, _blurHShaderID);
		//renderer::setRenderTarget(_tempRT);
		//renderer::draw_render_target(_baseRT, m_BlurHShaderID);
		renderer::restoreBackBuffer();
		renderer::draw_render_target(_tempRT, _blurHShaderID);
	}
}

}