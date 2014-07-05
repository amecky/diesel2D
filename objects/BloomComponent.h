#pragma once
#include "..\renderer\render_types.h"
#include "..\base\GameObject.h"
#include "..\compiler\Converter.h"

namespace ds {

const int SAMPLE_COUNT = 15;

// -------------------------------------------------------
// Bloom component converter
// -------------------------------------------------------
class BloomComponentConverter : public Converter {

public:
	BloomComponentConverter(void);
	~BloomComponentConverter(void);
	void convert(JSONReader& reader,BinaryWriter& writer);

	const char* getResourceDirectory() {
		return "content\\resources";
	}
};

// -------------------------------------------------------
// Bloom settings
// -------------------------------------------------------
struct BloomSettings  {

	int firstTarget;
	int initialTexture;
	float threshold;
	float intensity;
	float originalIntensity;
	float saturation;
	float originalSaturation;
	float blurAmount;

};

// -------------------------------------------------------
// Bloom component
// -------------------------------------------------------
class BloomComponent : public GameObject , public Serializer {

public:
	BloomComponent(void);
	~BloomComponent(void);
	void update(float elapsed);
	void init();
	void render();
	void load(BinaryLoader* loader);
	void useBloom(bool use) {
		m_UseBloom = use;
	}
private:
	BloomSettings m_Settings;
	int m_BloomTexture;
	int m_BlurHTexture;
	int m_BloomCombineTexture;
	Sprite m_BloomEntity;
	Sprite m_BlurHEntity;
	Sprite m_BloomCombineEntity;
	Sprite m_OverlayEntity;
	Sprite m_StraightEntity;
	Shader m_BlurHShader;
	Shader m_BlurVShader;
	Shader m_BloomShader;
	Shader m_BloomCombineShader;
	int m_BloomShaderID;
	int m_BlurHShaderID;
	int m_BloomCombineShaderID;
	bool m_UseBloom;
};


}
