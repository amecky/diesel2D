#pragma once
#include "RenderEffect.h"

namespace ds {
	
	class LightningRenderEffect : public RenderEffect {

		struct Light {
			int id;
			v2 pos;
			float radius;
		};

	public:
		LightningRenderEffect();
		~LightningRenderEffect();
		void start();
		void tick(float dt);
		void render();
		void onActivation();
		void setFactor(float f);
		DynamicGameSettings* getSettings() {
			return 0;
		}
		int add(const v2& pos, float radius);
		void setPosition(int id, const v2& pos);
		void setColor(int id, const Color& color);
		void remove(int id);
	private:
		int _ids;
		Shader* createShader();
		int _shaderID;
		float _timer;
		RTID _rtID;
		RTID _lightRT;
		Shader* _shader;
		Array<Light> _lights;
	};

}

