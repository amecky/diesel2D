#include "BloomComponent.h"
#include "..\renderer\shader.h"
#include "..\utils\Log.h"
#include "..\utils\PlainTextReader.h"
#include "..\compiler\AssetCompiler.h"
#include "..\renderer\graphics.h"

namespace ds {

BloomComponentConverter::BloomComponentConverter() : Converter("BloomComponentConverter") {
}


BloomComponentConverter::~BloomComponentConverter(void) {
}

void BloomComponentConverter::convert(JSONReader& reader,BinaryWriter& writer) {
	Category* c = reader.getCategory("bloom");
	writer.startChunk(CHNK_BLOOM_COMPONENT,1);
	writer.write(c->getInt("first_target",0));
	writer.write(c->getFloat("threshold",0.0f));
	writer.write(c->getFloat("intensity",0.0f));
	writer.write(c->getFloat("original_intensity",0.0f));
	writer.write(c->getFloat("saturation",0.0f));
	writer.write(c->getFloat("original_saturation",0.0f));
	writer.write(c->getFloat("blur_amount",0.0f));
	writer.closeChunk();					
}

const int SC_X = 512.0f;
const int SC_Y = 384.0f;

BloomComponent::BloomComponent() : m_UseBloom(true) {
}


BloomComponent::~BloomComponent(void) {
}

int BloomComponent::init(int baseTarget,int texID) {
	Color clr(0, 0, 0, 255);
	assets::load("bloom",this,CVT_BLOOM_COMPONENT);
	_baseRT = baseTarget;
	int currentTarget = m_Settings.firstTarget;
	// Bloom
	m_BloomTexture = renderer::createRenderTarget(clr);
	LOG << "Bloom texture: " << m_BloomTexture.textureID;
	m_BloomShaderID = shader::createBloomShader(1, m_Settings.threshold);
	m_BloomShader = renderer::getShader(m_BloomShaderID);
	m_BlurHShaderID = shader::createBlurShader(m_BloomTexture.textureID);
	m_BlurHShader = renderer::getShader(m_BlurHShaderID);
	m_BlurHShader->setTexture("gTex", m_BloomTexture.textureID);
	m_BlurHShader->setFloat("BlurDistance", 1.0f / 1024.0f);
	m_BlurHTexture = renderer::createRenderTarget(clr);
	// Bloom combine	
	m_BloomCombineTexture = renderer::createRenderTarget(clr);
	m_BloomCombineShaderID = shader::createBloomCombineShader(texID, m_BlurHTexture.textureID);
	m_BloomCombineShader = renderer::getShader(m_BloomCombineShaderID);
	LOG << "bloom render target: " << (currentTarget + 2);
	return currentTarget + 2;
}

void BloomComponent::update(float elapsed) {
	m_BloomShader->setFloat("Threshold", m_Settings.threshold);
	m_BloomCombineShader->setFloat("BloomIntensity", m_Settings.intensity);
	m_BloomCombineShader->setFloat("OriginalIntensity", m_Settings.originalIntensity);
	m_BloomCombineShader->setFloat("BloomSaturation", m_Settings.saturation);
	m_BloomCombineShader->setFloat("OriginalSaturation", m_Settings.originalSaturation);
}

void BloomComponent::render() {
	if ( m_UseBloom ) {
		sprites::flush();
		int currentShader = renderer::getCurrentShaderID();
		// first step -> base RT using bloom shader
		//renderer::setRenderTarget(m_Settings.firstTarget);
		//renderer::draw_render_target(_baseRT, m_BloomShaderID);
		// second step -> blur 
		//renderer::setRenderTarget(m_Settings.firstTarget + 1);
		//renderer::draw_render_target(m_Settings.firstTarget, m_BlurHShaderID);
		// thord step -> combine original and this one
		//renderer::setRenderTarget(m_Settings.firstTarget + 2);
		//renderer::draw_render_target(m_Settings.firstTarget + 1, m_BloomCombineShaderID);
		//renderer::restoreBackBuffer();
		//renderer::setCurrentShader(currentShader);
		//renderer::draw_render_target(m_Settings.firstTarget + 2);
	}
}

void BloomComponent::load(BinaryLoader* loader) {
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == CHNK_BLOOM_COMPONENT ) {	
			loader->read(&m_Settings.firstTarget);
			loader->read(&m_Settings.threshold);
			loader->read(&m_Settings.intensity);
			loader->read(&m_Settings.originalIntensity);
			loader->read(&m_Settings.saturation);
			loader->read(&m_Settings.originalSaturation);
			loader->read(&m_Settings.blurAmount);
		}
		loader->closeChunk();
	}		
}


}