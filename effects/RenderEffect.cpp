#include "RenderEffect.h"


namespace ds {

	RenderEffect::RenderEffect() : _active(false) {		
	}

	RenderEffect::~RenderEffect() {
	}


	// ------------------------------------------
	// gray scale effect
	// ------------------------------------------
	GrayScaleEffect::GrayScaleEffect() {
		// create render target
		_rtID = renderer::createRenderTarget(Color(0, 0, 0, 0));
		// load shader
		_shader = shader::createFadeToGrayShader(-1);
	}

	GrayScaleEffect::~GrayScaleEffect() {

	}

	void GrayScaleEffect::start() {
		if (isActive()) {
			// set render target
			renderer::setRenderTarget(_rtID);
		}
	}

	void GrayScaleEffect::render() {
		if (isActive()) {
			// restore backbuffer
			renderer::restoreBackBuffer();
			// draw rendertarget
			_shader->setFloat("timer", _settings.factor);
			renderer::draw_render_target(_rtID, _shader->getID());
		}
	}

	void GrayScaleEffect::setFactor(float f) {
		_settings.factor = math::clamp(f, 0.0f, 1.0f);
	}

}
