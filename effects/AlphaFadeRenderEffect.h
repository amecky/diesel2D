#pragma once
#include "RenderEffect.h"

namespace ds {
	
	struct AlphaFadeEffectSettings : public DynamicGameSettings {

		float ttl;
		int mode;
		//tweening::TweeningType type;
		int tweeningType;

		AlphaFadeEffectSettings() {
			addFloat("ttl", &ttl, 1.0f);
			addInt("mode", &mode, 0);
			addInt("tweening_type", &tweeningType, 0);
		}

		const char* getFileName() const {
			return "alphafade.json";
		}
	};

	class AlphaFadeEffect : public RenderEffect {

	public:
		AlphaFadeEffect();
		~AlphaFadeEffect();
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
		AlphaFadeEffectSettings _settings;

	};

}

