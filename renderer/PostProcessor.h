#pragma once
#include "render_types.h"
#include "Renderer.h"
#include "..\nodes\ScreenOverlayNode.h"

namespace ds {

class PostProcessor {

public:
	PostProcessor(Renderer* renderer);
	~PostProcessor(void);
	void start();
	void render();
	int getTexture() {
		return m_Texture;
	}
	void setShader(int shader);
private:
	int m_Texture;
	int m_Shader;
	int m_Material;
	ScreenOverlayNode* m_ScreenNode;
	Renderer* m_Renderer;
};

}

