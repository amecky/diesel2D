#pragma once
#include "..\renderer\render_types.h"
#include "..\renderer\Renderer.h"

namespace ds {

class SpriteObject;
// -----------------------------------------------------------
// SpriteBatch
// -----------------------------------------------------------
class SpriteBatch {
	
public:
	SpriteBatch(Renderer* renderer,uint32 maxQuads,int textureID);
	virtual ~SpriteBatch(void);
	void setShader(int shaderID) {
		m_ShaderID = shaderID;
	}
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	void begin();
	void draw(const SpriteObject& spriteObject);
	void draw(const Vector2f& pos,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0));
	void draw(float x,float y,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0));
	void end();	
private:		
	bool m_Rendering;
	void prepareBuffer();	
	int m_BlendState;
	Renderer* m_Renderer;
	int m_Handle;
	int m_TextureID;
	int m_ShaderID;
	uint32 m_MaxVertices;
	uint32 m_MaxIndices;
	uint32 m_VertexCounter;
	uint32 m_Index;
	char* m_DataBuffer;
	uint32 m_BufferSize;
	uint32 m_StartIndices;
	float m_TextureWidth;
	float m_TextureHeight;
};

}

