#include "BloomComponent.h"

namespace ds {

#include "BloomComponent.h"

const int SC_X = 512.0f;
const int SC_Y = 384.0f;

BloomComponent::BloomComponent() : GameObject() {
}


BloomComponent::~BloomComponent(void) {
}

void BloomComponent::initialize(int startLayer,int baseTexture,BloomSettings* bloomSettings,int blendState) {
	/*
	m_Settings = bloomSettings;
	int layer = startLayer;
	// Bloom
	LOG(logINFO) << "Creating Bloom rendertarget at layer: " << layer;
	int bloomTexture = m_World->createRenderTarget(layer);
	int blid = m_World->createSpriteBatch(baseTexture,1024.0f,768.0f);
	//m_World->addSpriteEntity(layer,blid,&m_BloomEntity,SC_X,SC_Y,Rect(0,0,1024,768));
	int bloomShaderID = shader::createBloomShader(m_Renderer,baseTexture,m_Settings->bloomThreshold);
	m_BloomShader = m_Renderer->getShader(bloomShaderID);
	m_World->setSpriteBatchShader(blid,bloomShaderID);

	// horizontal blur
	int blurHShaderID = shader::createBlurShader(m_Renderer,bloomTexture);
	m_BlurHShader = m_Renderer->getShader(blurHShaderID);
	setBlurEffectParameters(m_BlurHShader,1.0f/1024.0f,0.0f);
	++layer;
	LOG(logINFO) << "Creating horizontal blur at layer: " << layer;
	int blurHTexture = m_World->createRenderTarget(layer);
	int bhid = m_World->createSpriteBatch(blurHTexture,1024.0f,768.0f);
	//m_World->addSpriteEntity(layer,bhid,&m_BlurHEntity,SC_X,SC_Y,Rect(0,0,1024,768));	
	m_World->setSpriteBatchShader(bhid,blurHShaderID);
	// vertical blur
	int blurVShaderID = shader::createBlurShader(m_Renderer,blurHTexture);
	m_BlurVShader = m_Renderer->getShader(blurVShaderID);
	setBlurEffectParameters(m_BlurVShader,0.0f,1.0f/768.0f);
	++layer;
	LOG(logINFO) << "Creating vertical blur at layer: " << layer;
	int blurVTexture = m_World->createRenderTarget(layer);
	int bvid = m_World->createSpriteBatch(blurVTexture,1024.0f,768.0f);
	//m_World->addSpriteEntity(layer,bvid,&m_BlurVEntity,SC_X,SC_Y,Rect(0,0,1024,768));	
	m_World->setSpriteBatchShader(bvid,blurVShaderID);
	// Bloom combine	
	++layer;
	LOG(logINFO) << "Creating Bloom combine at layer: " << layer;
	int bloomCombineTexture = m_World->createRenderTarget(layer);
	int bcid = m_World->createSpriteBatch(bloomCombineTexture);
	//m_World->addSpriteEntity(layer,bcid,&m_BloomCombineEntity,SC_X,SC_Y,Rect(0,0,1024,768));
	int bcShaderID = shader::createBloomCombineShader(m_Renderer,baseTexture,blurVTexture);
	m_BloomCombineShader = m_Renderer->getShader(bcShaderID);
	m_World->setSpriteBatchShader(bcid,bcShaderID);
	// put to backbuffer
	++layer;
	LOG(logINFO) << "Creating Overlay at layer: " << layer;
	int oid = m_World->createSpriteBatch(bloomCombineTexture,1024.0f,768.0f,blendState);	
	//m_World->addSpriteEntity(layer,oid,&m_OverlayEntity,SC_X,SC_Y,Rect(0,0,1024,768));
	*/
}

void BloomComponent::setState(bool active) {
	//m_BloomEntity.setActive(active);
	//m_BloomCombineEntity.setActive(active);
	//m_OverlayEntity.setActive(active);
	//m_BlurHEntity.setActive(active);
	//m_BlurVEntity.setActive(active);
	
}

void BloomComponent::update(float elapsed) {
	shader::setFloat(m_BloomShader,"Threshold",m_Settings->bloomThreshold);
	shader::setFloat(m_BloomCombineShader,"BloomIntensity",m_Settings->bloomIntensity);
	shader::setFloat(m_BloomCombineShader,"OriginalIntensity",m_Settings->originalIntensity);
	shader::setFloat(m_BloomCombineShader,"BloomSaturation",m_Settings->bloomSaturation);
	shader::setFloat(m_BloomCombineShader,"OriginalSaturation",m_Settings->originalSaturation);
}

void BloomComponent::setBlurEffectParameters(Shader& shader,float dx, float dy) {	
	// Look up how many samples our gaussian blur effect supports.
	int sampleCount = SAMPLE_COUNT;
	// Create temporary arrays for computing our filter settings.
	float sampleWeights[SAMPLE_COUNT];
	Vector2f sampleOffsets[SAMPLE_COUNT];
	// The first sample always has a zero offset.
	sampleWeights[0] = computeGaussian(0);
	sampleOffsets[0] = Vector2f(0,0);
	// Maintain a sum of all the weighting values.
	float totalWeights = sampleWeights[0];
	// Add pairs of additional sample taps, positioned
	// along a line in both directions from the center.
	for (int i = 0; i < sampleCount / 2; i++) {
		// Store weights for the positive and negative taps.
		float weight = computeGaussian(i + 1);
		sampleWeights[i * 2 + 1] = weight;
		sampleWeights[i * 2 + 2] = weight;
		totalWeights += weight * 2;
		// To get the maximum amount of blurring from a limited number of
		// pixel shader samples, we take advantage of the bilinear filtering
		// hardware inside the texture fetch unit. If we position our texture
		// coordinates exactly halfway between two texels, the filtering unit
		// will average them for us, giving two samples for the price of one.
		// This allows us to step in units of two texels per sample, rather
		// than just one at a time. The 1.5 offset kicks things off by
		// positioning us nicely in between two texels.
		float sampleOffset = i * 2 + 1.5f;
		Vector2f delta = Vector2f(dx, dy) * sampleOffset;
		// Store texture coordinate offsets for the positive and negative taps.
		sampleOffsets[i * 2 + 1] = delta;
		sampleOffsets[i * 2 + 2] = -delta;
	}
	// Normalize the list of sample weightings, so they will always sum to one.
	for (int i = 0; i < SAMPLE_COUNT; i++) 	{
		sampleWeights[i] /= totalWeights;
	}
	/*
	for (int i = 0; i < SAMPLE_COUNT; i++) 	{
		LOG(logINFO) << i << "=" << sampleWeights[i];
	}

	for (int i = 0; i < SAMPLE_COUNT; i++) 	{		
		LOG(logINFO) << i << "=" << DBG_V2(sampleOffsets[i]);
	}
	*/
	shader::setValue(shader,"SampleWeights",&sampleWeights,sizeof(float)*SAMPLE_COUNT);
	shader::setValue(shader,"SampleOffsets",&sampleOffsets,sizeof(Vector2f)*SAMPLE_COUNT);
}

float BloomComponent::computeGaussian(float n) {
	float theta = m_Settings->blurAmount;
	return (float)((1.0 / sqrt(2 * PI * theta)) * exp(-(n * n) / (2 * theta * theta)));
}


}