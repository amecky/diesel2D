#include "PostProcessor.h"

namespace ds {

PostProcessor::PostProcessor(Renderer* renderer) : m_Renderer(renderer) {
	m_Texture = renderer->createRenderTarget("PPRT");
	m_Material = renderer->createMaterial("RTMaterial",m_Texture);
	/*
	m_Shader = new ds::TransformedTextureColorShader(renderer);
	m_Shader->init();
	m_Shader->setTexture("gTex",m_Texture);
	m_Material->shader = m_Shader;
	*/
	m_ScreenNode = new ScreenOverlayNode("Screen",renderer,m_Material);
}

void PostProcessor::setShader(int shader) {
	//m_Material->shader = shader;
}

PostProcessor::~PostProcessor(void) {
}

void PostProcessor::start() {
	m_Renderer->setRenderTarget("PPRT");
}

void PostProcessor::render() {
	m_Renderer->restoreBackBuffer();
	m_Renderer->drawNode(m_ScreenNode);
}

}
