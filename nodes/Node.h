#pragma once
#include "NodeTransformation.h"
#include "..\renderer\render_types.h"

namespace ds {

class Renderer;

class Node : public NodeTransformation {

public:
	Node(const char* nodeName,Renderer* renderer,int materialID);
	virtual ~Node() {};	
	bool isActive() const { 
		return m_Active;
	}
	void setActive(bool active) { 
		m_Active = active; 
	}
	// basic methods
	virtual void init() {}
	virtual void update(float elapsed) {}
	//virtual void render();
	// rendering
	virtual void prepareRendering() {}
	virtual void draw() {}
	virtual void postRendering() {}
	// debug
	virtual void debug() {}
	const char* getNodeName() const {
		return m_NodeName;
	}
	int getMaterial() {
		return m_Material;
	}
protected:
	Renderer* m_Renderer;
	//Material* m_Material;
	int m_Material;
	const char* m_NodeName;
	bool m_Active;
};

}

