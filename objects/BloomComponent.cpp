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

void BloomComponentConverter::convert(const char* fileName) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",fileName);
	char out[256];
	sprintf(out,"data\\%s.dsb",fileName);
	BinaryWriter writer;
	int signature[] = {0,8,15};
	writer.open(out,signature,3);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
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
		writer.close();
	}
	else {
		LOGEC("BloomComponentConverter") << "Cannot parse " << fileName;
	}
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
	m_BloomTexture = m_Renderer->createRenderTarget(currentTarget,Color::BLACK);
	LOG << "Bloom texture: " << m_BloomTexture;
	m_BloomShaderID = shader::createBloomShader(m_Renderer,m_Settings.initialTexture,m_Settings.threshold);
	m_BloomShader = m_Renderer->getShader(m_BloomShaderID);
	m_BloomEntity.setTextureRect(ds::Rect(0,0,1024,768),1024,768,m_Settings.initialTexture);
	m_BloomEntity.setPosition(Vector2f(512,384));
	
	++currentTarget;
	m_BlurHShaderID = m_Renderer->loadShader("blur","BlurTech");
	m_BlurHShader = m_Renderer->getShader(m_BlurHShaderID);
	shader::setTexture(m_BlurHShader,"gTex",m_Renderer,m_BloomTexture);
	shader::setFloat(m_BlurHShader,"BlurDistance",1.0f/1024.0f);
	m_BlurHTexture = m_Renderer->createRenderTarget(currentTarget,Color::BLACK);
	m_BlurHEntity.setTextureRect(ds::Rect(0,0,1024,768),1024,768,m_BloomTexture);
	m_BlurHEntity.setPosition(Vector2f(512,384));
	
	// Bloom combine	
	++currentTarget;
	m_BloomCombineTexture = m_Renderer->createRenderTarget(currentTarget,Color::BLACK);
	m_BloomCombineShaderID = shader::createBloomCombineShader(m_Renderer,m_Settings.initialTexture,m_BlurHTexture);
	m_BloomCombineShader = m_Renderer->getShader(m_BloomCombineShaderID);
	m_BloomCombineEntity.setTextureRect(ds::Rect(0,0,1024,768),1024,768,m_BlurHTexture);
	m_BloomCombineEntity.setPosition(Vector2f(512,384));

	m_OverlayEntity.setTextureRect(ds::Rect(0,0,1024,768),1024,768,m_BloomCombineTexture);
	m_OverlayEntity.setPosition(Vector2f(512,384));

	m_StraightEntity.setTextureRect(ds::Rect(0,0,1024,768),1024,768,m_Settings.initialTexture);
	m_StraightEntity.setPosition(Vector2f(512,384));
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
		int currentShader = m_Renderer->getCurrentShaderID();
		m_Renderer->setRenderTarget(m_Settings.firstTarget);
		m_Renderer->setCurrentShader(m_BloomShaderID);
		m_Renderer->draw(m_BloomEntity);
	
		m_Renderer->setRenderTarget(m_Settings.firstTarget+1);
		m_Renderer->setCurrentShader(m_BlurHShaderID);
		m_Renderer->draw(m_BlurHEntity);

		m_Renderer->setRenderTarget(m_Settings.firstTarget+2);
		m_Renderer->setCurrentShader(m_BloomCombineShaderID);
		m_Renderer->draw(m_BloomCombineEntity);

		m_Renderer->restoreBackBuffer(m_Settings.firstTarget);
		m_Renderer->setCurrentShader(currentShader);
		m_Renderer->draw(m_OverlayEntity);
	}
	else {
		m_Renderer->restoreBackBuffer(m_Settings.firstTarget);
		m_Renderer->draw(m_StraightEntity);
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