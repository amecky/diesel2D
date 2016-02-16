#pragma once
#include "..\renderer\graphics.h"
#include "..\data\DynamicSettings.h"

namespace ds {


	// -----------------------------------------
	// Render effect
	// -----------------------------------------
	class RenderEffect {

	public:
		RenderEffect();
		~RenderEffect();
		virtual void start() = 0;
		virtual void tick(float dt) {}
		virtual void render() = 0;
		virtual void onActivation() {}
		virtual void onDeactivation() {}
		const bool isActive() const {
			return _active;
		}
		void activate() {
			_active = true;
			onActivation();
		}
		void deactivate() {
			_active = false;
			onDeactivation();
		}
		virtual DynamicGameSettings* getSettings() = 0;
	private:
		bool _active;
	};

	// -----------------------------------------
	// gray scale effect
	// -----------------------------------------
	struct GrayScaleEffectSettings : public DynamicGameSettings {

		float factor;

		GrayScaleEffectSettings() {
			addFloat("factor", &factor, 1.0f);
		}

		const char* getFileName() const {
			return "grayscale.json";
		}
	};

	class GrayScaleEffect : public RenderEffect {

	public:
		GrayScaleEffect();
		~GrayScaleEffect();
		void start();
		void tick(float dt) {}
		void render();
		void onActivation() {}
		void setFactor(float f);
		DynamicGameSettings* getSettings() {
			return &_settings;
		}
	private:
		RTID _rtID;
		Shader* _shader;
		GrayScaleEffectSettings _settings;

	};

}

