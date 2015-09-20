#pragma once
#include "..\renderer\render_types.h"
#include "..\compiler\Converter.h"
#include "..\sprites\Sprite.h"
#include "..\sprites\SpriteBatch.h"
#include "..\io\Serializer.h"

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
class BloomComponent : public Serializer {

public:
	BloomComponent(void);
	~BloomComponent(void);
	void update(float elapsed);
	int init(int baseTarget,int texID);
	void render();
	void load(BinaryLoader* loader);
	void useBloom(bool use) {
		m_UseBloom = use;
	}
	void toggle() {
		m_UseBloom = !m_UseBloom;
	}
	const bool isActive() const {
		return m_UseBloom;
	}
private:
	BloomSettings m_Settings;
	int m_BloomTexture;
	int m_BlurHTexture;
	int m_BloomCombineTexture;
	Shader* m_BlurHShader;
	Shader* m_BlurVShader;
	Shader* m_BloomShader;
	Shader* m_BloomCombineShader;
	int m_BloomShaderID;
	int m_BlurHShaderID;
	int m_BloomCombineShaderID;
	bool m_UseBloom;
	int _baseRT;
};


}
