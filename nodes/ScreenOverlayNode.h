#pragma once
#include "..\utils\GameMath.h"
#include <vector>
#include "..\nodes\Node.h"
#include "..\renderer\IndexVertexBuffer.h"
#include "..\renderer\render_types.h"

namespace ds {

class Renderer;

class ScreenOverlayNode : public Node {

public:
	ScreenOverlayNode(const char* name,Renderer* renderer,int material);
	virtual ~ScreenOverlayNode();
	void prepareRendering();
	void draw();
	void postRendering();
private:		
	TTCBuffer* m_Buffer;
};

};

