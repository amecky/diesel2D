#pragma once
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\render_types.h"
#include "..\particles\Particle.h"
namespace ds {

struct MySprite {

	uint32 id;
	Texture texture;
	float timer;
	Vector2f position;
	Vector2f velocity;
	Vector2f target;
	int index;
	float radius;
	float angle;
	Vector2f scale;
	Color color;
	bool active;
	int layer;
};

class Sprite;
class Renderer;

namespace gfx {

	void init(Renderer* renderer,int shaderID,int blendStateID);

	void draw(int textureID, const NParticleArray& particleArray);

}

namespace sprites {

	void init(Renderer* renderer);

	void draw(const Vector2f& pos, int textureID, const Vector4f& uv, const Vector2f& dim, float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, const Color& color = Color::WHITE, const Vector2f& center = Vector2f(0, 0));

	void draw(const Sprite& sprite);

	void draw(const MySprite& sprite);

	void begin();

	void flush();

	void create(MySprite* sprite,int layer,const Vector2f& pos, const Rect& r, float rotation = 0.0f, const Vector2f& scale = Vector2f(1, 1), const Color& clr = Color::WHITE);
}
// -----------------------------------------------------------
// SpriteBatch
// -----------------------------------------------------------
class SpriteBatch {
	
public:
	SpriteBatch(Renderer* renderer);
	virtual ~SpriteBatch();
	void draw(int textureID, const Rect& textureRect, const ParticleArray& particleArray);	
};

}

