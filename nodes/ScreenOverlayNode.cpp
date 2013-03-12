#include "ScreenOverlayNode.h"
#include "..\renderer\Renderer.h"

namespace ds {

ScreenOverlayNode::ScreenOverlayNode(const char* name,Renderer* renderer,int material) : Node(name,renderer,material) {
	m_Buffer = new TTCBuffer(renderer,12,20,false);	
	m_Buffer->clear();
	float u1,u2,v1,v2;
	u1 = v1 = 0.0f;
	u2 = v2 = 1.0f;
	int width = renderer->getWidth();
	int height = renderer->getHeight();	
	m_Buffer->add(TTCVertex(      - 0.5f,        - 0.5f,0.0f,u1,v1));
	m_Buffer->add(TTCVertex(width - 0.5f,        - 0.5f,0.0f,u2,v1));
	m_Buffer->add(TTCVertex(width - 0.5f, height - 0.5f,0.0f,u2,v2));
	m_Buffer->add(TTCVertex(      - 0.5f, height - 0.5f,0.0f,u1,v2));	

	m_Buffer->addIndex(0);
	m_Buffer->addIndex(1);
	m_Buffer->addIndex(2);
	m_Buffer->addIndex(2);
	m_Buffer->addIndex(3);
	m_Buffer->addIndex(0);

	m_Buffer->update();
}

ScreenOverlayNode::~ScreenOverlayNode() {
}

void ScreenOverlayNode::prepareRendering() {
	//gEngine->getRenderer()->getDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	//gEngine->getRenderer()->getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	//gEngine->getRenderer()->getDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_Buffer->prepareData();
}

void ScreenOverlayNode::draw() {
	m_Buffer->render(2);
}

void ScreenOverlayNode::postRendering() {
	//d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	//d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	//d3ddevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
}

};
