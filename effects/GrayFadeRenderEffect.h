#pragma once
#include "RenderEffect.h"

namespace ds {
	
	struct GrayFadeEffectSettings : public DynamicGameSettings {

		float ttl;
		int mode;
		//tweening::TweeningType type;
		int tweeningType;

		GrayFadeEffectSettings() {
			addFloat("ttl", &ttl, 1.0f);
			addInt("mode", &mode, 1);
			addInt("tweening_type", &tweeningType, 1);
		}

		const char* getFileName() const {
			return "grayfade.json";
		}
	};

	class GrayFadeEffect : public RenderEffect {

	public:
		GrayFadeEffect();
		~GrayFadeEffect();
		void start();
		void tick(float dt);
		void render();
		void onActivation();
		void setFactor(float f);
		DynamicGameSettings* getSettings() {
			return &_settings;
		}
	private:
		Shader* createShader();
		float _timer;
		RTID _rtID;
		Shader* _shader;
		GrayFadeEffectSettings _settings;

	};

}

