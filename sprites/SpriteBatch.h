#pragma once
#include "..\renderer\render_types.h"
#include "..\particles\Particle.h"
#include "..\lib\DataContainer.h"
#include "Sprite.h"
#include "..\renderer\shader.h"

namespace ds {

//struct Sprite;
class Renderer;

namespace sprites {

	void init();

	void initializeTextSystem(int textureID, const char* fontName);

	int getDescriptorID();

	void setDescriptorID(int id);

	void draw(const Vector2f& pos, int textureID, const Vector4f& uv, const Vector2f& dim, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE, const Vector2f& center = Vector2f(0, 0));

	void draw(const Vector2f& pos,const Texture& tex, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE, const Vector2f& center = Vector2f(0, 0));

	void draw(const Sprite& sprite);

	void draw(const Shape& shape,const Texture& tex);

	void draw(const DataContainer& container,const Texture& texture,int posIdx);

	void draw(const DataContainer& container,const Texture& texture,int posIdx,int angleIdx);

	void draw(const DataContainer& container,const Texture& texture,int posIdx,int angleIdx,int scaleIdx);

	void draw(const Texture& texture,const ParticleArray& array);

	void drawTemplate(int id);

	void drawText(int x, int y, const char* text,float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE);

	void setShaderID(int shaderID);

	void begin();

	void flush();

}

}

