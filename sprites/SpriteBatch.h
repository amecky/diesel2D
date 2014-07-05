#pragma once
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\render_types.h"

namespace ds {

class Sprite;
class Renderer;
// -----------------------------------------------------------
// SpriteBatch
// -----------------------------------------------------------
class SpriteBatch {
	
public:
	SpriteBatch(Renderer* renderer);
	virtual ~SpriteBatch();
	void setShader(int shaderID) {
		if ( m_ShaderID != shaderID ) {
			flush();
		}
		m_ShaderID = shaderID;
	}
	void setBlendState(int blendState);
	void begin();
	void draw(const Sprite& spriteObject);
	void draw(const Vector2f& pos,int textureID,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0));
	void draw(float x,float y,int textureID,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vector2f& center = Vector2f(0,0));
	void end();	
	void prepare() {
		m_CurrentTextureID = -1;
		m_CurrentBlendState = -1;
		m_ShaderID = -1;
	}
	void flush() {
		end();
		begin();
	}

	void setTextureID(int textureID);
private:		
	bool m_Rendering;
	void prepareBuffer();	
	Renderer* m_Renderer;
	int m_Handle;
	int m_CurrentTextureID;
	int m_CurrentBlendState;
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

