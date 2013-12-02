#pragma once
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\IndexVertexBuffer.h"
#include "..\renderer\render_types.h"

namespace ds {


// -----------------------------------------------------------
// SpriteBatch
// -----------------------------------------------------------
class SpriteBatch {
	
public:
	SpriteBatch(Renderer* renderer,uint32 maxQuads,int textureID);
	virtual ~SpriteBatch(void);
	//void createMaterial(int textureID);
	void setShader(int shaderID) {
		m_ShaderID = shaderID;
	}
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	//const int getMaterial()  const {
		//return m_Mtrl;
	//}
	void begin();
	void draw(const Sprite& sprite);
	void draw(const Vec2& pos,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vec2& center = Vec2(0,0));
	void draw(float x,float y,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE,const Vec2& center = Vec2(0,0));
	void end();	
private:		
	void prepareBuffer();	
	int m_BlendState;
	Renderer* m_Renderer;
	int m_Handle;
	int m_TextureID;
	int m_ShaderID;
	//int m_Mtrl;
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

