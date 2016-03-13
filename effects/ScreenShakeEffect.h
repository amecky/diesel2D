#pragma once
#include "RenderEffect.h"

namespace ds {
	
	struct ScreenShakeEffectSettings : public DynamicGameSettings {

		float ttl;
		float frequency;
		float amplitude;

		ScreenShakeEffectSettings() {
			addFloat("ttl", &ttl, 1.0f);
			addFloat("frequency", &frequency, 1.0f);
			addFloat("amplitude", &amplitude, 1.0f);
		}

		const char* getFileName() const {
			return "screenshake.json";
		}
	};

	class ScreenShakeEffect : public RenderEffect {

	public:
		ScreenShakeEffect();
		~ScreenShakeEffect();
		void start();
		void tick(float dt);
		void render();
		void onActivation();
		void setFactor(float f);
		DynamicGameSettings* getSettings() {
			return &_settings;
		}
	private:
		ds::Shader* createShader();
		float _timer;
		RTID _rtID;
		Shader* _shader;
		ScreenShakeEffectSettings _settings;

	};

}

