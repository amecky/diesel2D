#pragma once
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\render_types.h"

namespace ds {

class SpriteObject;
class Renderer;
// -----------------------------------------------------------
// SpriteBatch
// -----------------------------------------------------------
class NewSpriteBatch {
	
public:
	NewSpriteBatch(Renderer* renderer);
	virtual ~NewSpriteBatch();
	void setShader(int shaderID) {
		m_ShaderID = shaderID;
	}
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	void begin();
	void draw(const SpriteObject& spriteObject);
	void draw(const Vector2f& pos,int textureID,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0));
	void draw(float x,float y,int textureID,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0));
	void end();	
	void prepare() {
		m_CurrentTextureID = -1;
	}
private:		
	bool m_Rendering;
	void prepareBuffer();	
	int m_BlendState;
	Renderer* m_Renderer;
	int m_Handle;
	int m_CurrentTextureID;
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

