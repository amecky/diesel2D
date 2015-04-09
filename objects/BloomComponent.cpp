#include "BloomComponent.h"
#include "..\renderer\shader.h"
#include "..\utils\Log.h"
#include "..\utils\PlainTextReader.h"
#include "..\compiler\AssetCompiler.h"

namespace ds {

BloomComponentConverter::BloomComponentConverter() : Converter("BloomComponentConverter") {
}


BloomComponentConverter::~BloomComponentConverter(void) {
}

void BloomComponentConverter::convert(JSONReader& reader,BinaryWriter& writer) {
	Category* c = reader.getCategory("bloom");
	writer.startChunk(CHNK_BLOOM_COMPONENT,1);
	writer.write(c->getInt("initial_texture",0));
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

BloomComponent::BloomComponent() : GameObject() , m_UseBloom(true) {
}


BloomComponent::~BloomComponent(void) {
}

void BloomComponent::init() {
	
	m_Assets->load("bloom",this,CVT_BLOOM_COMPONENT);
	int currentTarget = m_Settings.firstTarget;
	// Bloom
	m_BloomTexture = renderer::createRenderTarget(currentTarget,Color::BLACK);
	LOG << "Bloom texture: " << m_BloomTexture;
	m_BloomShaderID = shader::createBloomShader(m_Renderer,m_Settings.initialTexture,m_Settings.threshold);
	m_BloomShader = renderer::getShader(m_BloomShaderID);
	m_BloomEntity.texture = math::buildTexture(0,0,1024,768,1024,768,m_Settings.initialTexture);
	m_BloomEntity.position = Vector2f(512,384);
	
	++currentTarget;
	m_BlurHShaderID = renderer::loadShader("blur", "BlurTech");
	m_BlurHShader = renderer::getShader(m_BlurHShaderID);
	shader::setTexture(m_BlurHShader,"gTex",m_BloomTexture);
	shader::setFloat(m_BlurHShader,"BlurDistance",1.0f/1024.0f);
	m_BlurHTexture = renderer::createRenderTarget(currentTarget,Color::BLACK);
	m_BlurHEntity.texture = math::buildTexture(0, 0, 1024, 768, 1024, 768, m_BloomTexture);
	m_BlurHEntity.position = Vector2f(512,384);
	
	// Bloom combine	
	++currentTarget;
	m_BloomCombineTexture = renderer::createRenderTarget(currentTarget, Color::BLACK);
	m_BloomCombineShaderID = shader::createBloomCombineShader(m_Renderer,m_Settings.initialTexture,m_BlurHTexture);
	m_BloomCombineShader = renderer::getShader(m_BloomCombineShaderID);
	m_BloomCombineEntity.texture = math::buildTexture(0, 0, 1024, 768, 1024, 768, m_BlurHTexture);
	m_BloomCombineEntity.position = Vector2f(512,384);

	m_OverlayEntity.texture = math::buildTexture(0, 0, 1024, 768, 1024, 768, m_BloomCombineTexture);
	m_OverlayEntity.position = Vector2f(512,384);

	m_StraightEntity.texture = math::buildTexture(0, 0, 1024, 768, 1024, 768, m_Settings.initialTexture);
	m_StraightEntity.position = Vector2f(512,384);
}

void BloomComponent::update(float elapsed) {
	shader::setFloat(m_BloomShader,"Threshold",m_Settings.threshold);
	shader::setFloat(m_BloomCombineShader,"BloomIntensity",m_Settings.intensity);
	shader::setFloat(m_BloomCombineShader,"OriginalIntensity",m_Settings.originalIntensity);
	shader::setFloat(m_BloomCombineShader,"BloomSaturation",m_Settings.saturation);
	shader::setFloat(m_BloomCombineShader,"OriginalSaturation",m_Settings.originalSaturation);
}

void BloomComponent::render() {
	if ( m_UseBloom ) {
		int currentShader = renderer::getCurrentShaderID();
		renderer::setRenderTarget(m_Settings.firstTarget);
		renderer::setCurrentShader(m_BloomShaderID);
		sprites::draw(m_BloomEntity);
	
		renderer::setRenderTarget(m_Settings.firstTarget + 1);
		renderer::setCurrentShader(m_BlurHShaderID);
		sprites::draw(m_BlurHEntity);

		renderer::setRenderTarget(m_Settings.firstTarget + 2);
		renderer::setCurrentShader(m_BloomCombineShaderID);
		sprites::draw(m_BloomCombineEntity);

		renderer::restoreBackBuffer();
		renderer::setCurrentShader(currentShader);
		sprites::draw(m_OverlayEntity);
	}
	else {
		renderer::restoreBackBuffer();
		sprites::draw(m_StraightEntity);
	}
}

void BloomComponent::load(BinaryLoader* loader) {
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == CHNK_BLOOM_COMPONENT ) {	
			loader->read(&m_Settings.initialTexture);
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