#include "Node.h"
#include "..\renderer\Renderer.h"
#include "..\utils\Profiler.h"

namespace ds {

Node::Node(const char* nodeName,Renderer* renderer,int materialID) 
	: NodeTransformation() , m_NodeName(nodeName) , m_Renderer(renderer) , m_Material(materialID) {
}

// -------------------------------------------------------
// Render the node
// -------------------------------------------------------
/*
void Node::render() {
	PR_START(m_NodeName)
	m_Renderer->applyTransformation(this);
	PR_START("PREPARE")
	prepareRendering();
	PR_END("PREPARE")
	PR_START("DRAW")
	m_Renderer->applyMaterial(m_Material);	
	m_Renderer->drawNode(this);
	PR_END("DRAW")
	PR_START("POST")
	postRendering();
	PR_END("POST")	
	PR_END(m_NodeName)
}
*/
};
