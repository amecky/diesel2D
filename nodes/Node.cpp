#include "Node.h"
#include "..\renderer\Renderer.h"
#include "..\utils\Profiler.h"

namespace ds {

Node::Node(const char* nodeName,Renderer* renderer,int materialID) 
	: m_NodeName(nodeName) , m_Renderer(renderer) , m_Material(materialID) {
}

};
