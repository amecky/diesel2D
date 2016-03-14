#pragma once
#include "..\renderer\graphics.h"
#include "..\data\DynamicSettings.h"

namespace ds {

	enum EffectTypes {
		ET_SCREEN_SHAKE,
		ET_GRAY_FADE,
		ET_BLUR,
		ET_PIXELATE
	};
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

	

}

