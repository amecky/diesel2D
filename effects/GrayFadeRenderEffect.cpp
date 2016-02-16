#include "GrayFadeRenderEffect.h"

namespace ds {

	// ------------------------------------------
	// gray fade effect
	// ------------------------------------------
	GrayFadeEffect::GrayFadeEffect() {
		// create render target
		_rtID = renderer::createRenderTarget(Color(0, 0, 0, 0));
		// load shader
		_shader = shader::createFadeToGrayShader(-1);
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

}
