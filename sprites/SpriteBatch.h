#pragma once
#include "..\renderer\render_types.h"
#include "..\particles\Particle.h"
#include "Sprite.h"
#include "..\renderer\shader.h"

namespace ds {

struct BitmapFont;
class Renderer;

namespace sprites {

	void init();

	void initializeTextSystem(BitmapFont * font);

	int getDescriptorID();

	void setDescriptorID(int id);

	void draw(const Vector2f& pos, int textureID, const Vector4f& uv, const Vector2f& dim, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE, const Vector2f& center = Vector2f(0, 0));

	void draw(const Vector2f& pos,const Texture& tex, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE, const Vector2f& center = Vector2f(0, 0));

	void draw(const Sprite& sprite);

	void draw(const BasicSprite& sprite);

	void draw(const Shape& shape,const Texture& tex);

	void draw(const Texture& texture,const ParticleArray& array);

	void drawTiledX(const v2& position, float width, const Texture& texture, float cornersize, const Color& color = Color::WHITE);

	void drawTiledXY(const v2& position, const v2& size, const Texture& texture, float cornersize, const Color& color = Color::WHITE);

	void drawTemplate(const char* name);

	void drawText(int x, int y, const char* text,float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE);

	v2 getTextSize(const char* text, float scaleX = 1.0f, float scaleY = 1.0f);

	void drawText(BitmapFont* font,int x, int y, const char* text, int padding = 4, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE);

	void setShaderID(int shaderID);

	void begin();

	void flush(bool count = true);

	void setTexture(int textureID);

	int getCurrentTextureID();

	void shutdown();
}

}

