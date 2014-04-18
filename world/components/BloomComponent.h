#pragma once
#include "..\..\renderer\render_types.h"
#include "..\..\base\GameObject.h"

namespace ds {

const int SAMPLE_COUNT = 15;

struct BloomSettings  {

	float bloomThreshold;
	float bloomIntensity;
	float originalIntensity;
	float bloomSaturation;
	float originalSaturation;
	float blurAmount;

	void load() {
		/*
		reader.get<float>("bloom_threshold",&bloomThreshold);
		reader.get<float>("bloom_intensity",&bloomIntensity);
		reader.get<float>("original_intensity",&originalIntensity);
		reader.get<float>("bloom_saturation",&bloomSaturation);
		reader.get<float>("original_saturation",&originalSaturation);
		reader.get<float>("blur_amount",&blurAmount);
		*/
	}
};

class BloomComponent : public GameObject {

public:
	BloomComponent(void);
	~BloomComponent(void);
	void update(float elapsed);
	void init() {}
	void initialize(int startLayer,int baseTexture,BloomSettings* bloomSettings,int blendState = -1);
	void activate() {
		setState(true);
	}
	void deactivate() {
		setState(false);
	}
private:
	void setState(bool active);
	void setBlurEffectParameters(Shader& shader,float dx, float dy);
	float computeGaussian(float n);
	BloomSettings* m_Settings;
	//SpriteEntity m_BloomEntity;
	//SpriteEntity m_BlurHEntity;
	//SpriteEntity m_BlurVEntity;
	//SpriteEntity m_BloomCombineEntity;
	//SpriteEntity m_OverlayEntity;
	Shader m_BlurHShader;
	Shader m_BlurVShader;
	Shader m_BloomShader;
	Shader m_BloomCombineShader;
};


}
