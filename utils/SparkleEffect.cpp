#include "SparkleEffect.h"
#include "..\sprites\SpriteBatch.h"
#include "..\math\GameMath.h"

namespace ds {
	// --------------------------------------------
	// Sparkle effect
	// --------------------------------------------
	SparkleEffect::SparkleEffect(const SparkleEffectSettings& settings) : _settings(settings) {
		_sparkles.initialize(4096);
		_startX = (1024 - _settings.square_size * _settings.max_x) / 2;
		_startY = (768 - _settings.square_size * _settings.max_y) / 2;
	}


	SparkleEffect::~SparkleEffect() {
	}

	// --------------------------------------------
	// start block
	// --------------------------------------------
	void SparkleEffect::start(int xp, int yp, const Rect& r, int pieces) {
		int total = pieces * pieces;
		float dx = r.width() / pieces;
		float dy = r.height() / pieces;
		float gap = _settings.gap;
		int start = _sparkles.countAlive;
		int end = start + total;
		v2 np = v2(_startX + xp * _settings.square_size, _startY + yp * _settings.square_size);
		np.x -= (pieces - 1) * gap + _settings.square_size / 4;
		np.y += (pieces - 1) * gap + _settings.square_size / 4;
		if (end < _sparkles.count) {
			for (int i = 0; i < total; ++i) {
				_sparkles.wake(start + i);
			}
			int cnt = start;
			for (int y = 0; y < pieces; ++y) {
				for (int x = 0; x < pieces; ++x) {
					_sparkles.positions[cnt] = v2(np.x + (dx + gap) * x, np.y - (dy + gap) * y);
					_sparkles.timers[cnt] = 0.0f;
					_sparkles.textures[cnt] = math::buildTexture(r.top + dy * y, r.left + dx * x, dx, dy);
					float v = _settings.velocity + math::random(-_settings.velocityVariance, _settings.velocityVariance);
					float angle = math::getTargetAngle(_sparkles.positions[cnt], v2(512, 384));
					_sparkles.velocities[cnt] = math::getRadialVelocity(angle, v);
					++cnt;
				}
			}
		}
	}

	// --------------------------------------------
	// update
	// --------------------------------------------
	void SparkleEffect::update(float dt) {
		for (int i = 0; i < _sparkles.countAlive; ++i) {
			_sparkles.timers[i] += dt;
			if (_sparkles.timers[i] > _settings.ttl) {
				_sparkles.kill(i);
			}
		}
		for (int i = 0; i < _sparkles.countAlive; ++i) {
			_sparkles.positions[i] += _sparkles.velocities[i] * dt;
			_sparkles.scales[i] = lerp(v2(_settings.startScale, _settings.startScale), v2(_settings.endScale, _settings.endScale), _sparkles.timers[i] / _settings.ttl);
		}
	}

	// --------------------------------------------
	// render
	// --------------------------------------------
	void SparkleEffect::render() {
		for (int i = 0; i < _sparkles.countAlive; ++i) {
			sprites::draw(_sparkles.positions[i], _sparkles.textures[i], 0.0f, _sparkles.scales[i].x, _sparkles.scales[i].y);
		}
	}

	// --------------------------------------------
	// Intro effect
	// --------------------------------------------
	IntroEffect::IntroEffect(const SparkleEffectSettings& settings) : SparkleEffect(settings), _warmUpTimer(0.0f), _running(false) {}

	// --------------------------------------------
	// start
	// --------------------------------------------
	void IntroEffect::start(int xp, int yp, const Rect& r, int stepX, int stepY, float gap, float textureSize) {
		_texture = math::buildTexture(r,textureSize,textureSize);
		int total = stepX * stepY;
		float dx = r.width() / stepX;
		float dy = r.height() / stepY;
		int start = _sparkles.countAlive;
		int end = start + total;
		int xpos = (1024 - r.width()) / 2;
		int ypos = (768 - r.height()) / 2 + r.height();
		v2 np = v2(xpos, ypos);
		if (end < _sparkles.count) {
			for (int i = 0; i < total; ++i) {
				_sparkles.wake(start + i);
			}
			int cnt = start;
			for (int y = 0; y < stepY; ++y) {
				for (int x = 0; x < stepX; ++x) {
					_sparkles.positions[cnt] = v2(np.x + (dx + gap) * x, np.y - (dy + gap) * y);
					_sparkles.timers[cnt] = 0.0f;
					_sparkles.textures[cnt] = math::buildTexture(r.top + dy * y, r.left + dx * x, dx, dy,textureSize,textureSize);
					float angle = math::getTargetAngle(_sparkles.positions[cnt], v2(512, 384));
					//float angle = math::random(0.0f, TWO_PI);
					float v = _settings.velocity + math::random(-_settings.velocityVariance, _settings.velocityVariance);
					_sparkles.velocities[cnt] = math::getRadialVelocity(RADTODEG(angle), v);
					_sparkles.scales[cnt] = v2(1, 1);
					++cnt;
				}
			}
		}
		_warmUpTimer = _settings.warmupTimer;
		_running = true;
	}

	void IntroEffect::render() {
		if (_running) {
			if (_warmUpTimer > 0.0f) {
				sprites::draw(v2(512, 384), _texture);
			}
			else {
				SparkleEffect::render();
			}
		}
	}
	// --------------------------------------------
	// update
	// --------------------------------------------
	void IntroEffect::update(float dt) {
		if (_running) {
			if (_warmUpTimer > 0.0f) {
				_warmUpTimer -= dt;
			}
			else {
				for (int i = 0; i < _sparkles.countAlive; ++i) {
					_sparkles.timers[i] += dt;
					if (_sparkles.timers[i] > _settings.ttl) {
						_sparkles.kill(i);
					}
				}
				for (int i = 0; i < _sparkles.countAlive; ++i) {
					_sparkles.positions[i] += _sparkles.velocities[i] * dt;
					_sparkles.scales[i] = lerp(v2(_settings.startScale, _settings.startScale), v2(_settings.endScale, _settings.endScale), _sparkles.timers[i] / _settings.ttl);
				}
				if (_sparkles.countAlive == 0) {
					_running = false;
				}
			}
		}
	}

}