#include "SpriteBatch.h"
#include "..\renderer\vertex_types.h"
#include "..\utils\Profiler.h"
#include "..\renderer\shader.h"
#include "..\sprites\Sprite.h"
#include "..\utils\Log.h"
#include "..\utils\font.h"
#include "..\math\GameMath.h"
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\graphics.h"
#include "..\renderer\BatchBuffer.h"
#include "..\math\matrix.h"

namespace ds {

// ------------------------------------------------------------
//
// ------------------------------------------------------------
namespace sprites {

	const int MAX_SPRITES = 16386;

	struct SpriteBatchContext {

		BatchBuffer<ParticleVertex>* buffer;
		int descriptorID;
		BitmapFont* font;

		SpriteBatchContext() : font(0) {}

		~SpriteBatchContext() {}
	};

	static SpriteBatchContext* spriteCtx = 0;

	void init() {		
		assert(spriteCtx == 0);
		spriteCtx = new SpriteBatchContext;
		Descriptor desc;
		desc.shader = shader::createParticleShader();
		assert(desc.shader != 0);
		desc.texture = 0;
		desc.blendState = renderer::getDefaultBlendState();
		BatchBufferDescriptor descriptor;
		descriptor.maxItems = MAX_SPRITES * 4;
		descriptor.descriptorID = renderer::addDescriptor(desc);
		descriptor.vertexDeclaration = VD_QUAD;
		spriteCtx->buffer = new BatchBuffer<ParticleVertex>(descriptor);
		spriteCtx->descriptorID = descriptor.descriptorID;
	}

	void initializeTextSystem(BitmapFont * font) {
		spriteCtx->font = font;		
	}

	int getDescriptorID() {
		return spriteCtx->descriptorID;
	}

	void setDescriptorID(int id) {	
		
		// FIXME: correct order?????
		if (spriteCtx->descriptorID != id) {		
			flush();
			spriteCtx->descriptorID = id;			
		}
	}

	void begin() {
		spriteCtx->buffer->begin();
	}

	int getCurrentTextureID() {
		return renderer::getDescriptorData()->textures[spriteCtx->descriptorID];
	}

	void setTexture(int textureID) {
		flush();		
		spriteCtx->buffer->setTextureID(textureID);
		//renderer::getDescriptorData()->textures[spriteCtx->descriptorID] = textureID;
	}

	void setShaderID(int shader) {
		// the shader in desctiptor
		renderer::getDescriptorData()->shaders[spriteCtx->descriptorID] = shader;
		//flush();
	}

	void end(bool count) {
		spriteCtx->buffer->end();
	}

	void flush(bool count) {
		spriteCtx->buffer->flush();
	}

	void drawText(int x, int y, const char* text, float scaleX, float scaleY, const Color& color) {
		assert(spriteCtx->font != 0);
		float padding = 0.0f;
		int len = strlen(text);
		for (int cnt = 0; cnt < len; ++cnt) {
			char c = text[cnt];
			if (spriteCtx->font->contains(c)) {
				CharDef cd = spriteCtx->font->getCharDef(c);
				padding = (cd.width + 2)  * scaleX;
				float dimX = cd.width * scaleX;
				float dimY = spriteCtx->font->charHeight * scaleY;
				draw(Vector2f(x + dimX * 0.5f, y + dimY * 0.5f), cd.texture, 0.0f, scaleX, scaleY, color);
				x += dimX + 4;
			}
		}		
	}

	v2 getTextSize(const char* text, float scaleX, float scaleY) {
		return font::calculateSize(*spriteCtx->font, text, 4, scaleX, scaleY);
	}
	
	void drawText(BitmapFont* font,int x, int y, const char* text,int padding, float scaleX, float scaleY, const Color& color) {
		assert(font != 0);
		int len = strlen(text);
		for (int cnt = 0; cnt < len; ++cnt) {
			char c = text[cnt];
			if (font->contains(c)) {
				CharDef cd = font->getCharDef(c);				
				float dimX = cd.width * scaleX;
				float dimY = font->charHeight * scaleY;
				draw(v2(x + dimX * 0.5f, y + dimY * 0.5f),cd.texture, 0.0f, scaleX, scaleY, color);
				x += dimX + padding;
			}
		}
	}

	void draw(const Vector2f& pos, int textureID, const Vector4f& uv, const Vector2f& dim, float rotation, float scaleX, float scaleY, const Color& color, const Vector2f& center) {
		v2 ar[] = {
			v2(uv.x, uv.y),
			v2(uv.z, uv.y),
			v2(uv.z, uv.w),
			v2(uv.x, uv.w)
		};
		ParticleVertex v;
		for (int i = 0; i < 4; ++i) {
			v.x = pos.x;
			v.y = pos.y;
			v.z = 0.0f;
			v.uv = ar[i];
			v.scale.x = scaleX;
			v.scale.y = scaleY;
			v.dimension = dim;
			v.rotationIndex.x = rotation;
			v.rotationIndex.y = i;
			v.color = color;
			spriteCtx->buffer->append(v);
		}
	}

	void draw(const Shape& shape, const Texture& tex) {
		ParticleVertex v;
		for (int i = 0; i < 4; ++i) {
			v.x = shape.v[i].x;
			v.y = shape.v[i].y;
			v.z = 0.0f;
			v.uv = tex.getUV(i);
			v.scale.x = 1.0f;
			v.scale.y = 1.0f;
			v.dimension = tex.dim;
			v.rotationIndex.x = 0.0f;
			v.rotationIndex.y = i;
			v.color = shape.color;
			spriteCtx->buffer->append(v);
		}
	}

	void draw(const Vector2f& pos, const Texture& tex, float rotation, float scaleX, float scaleY, const Color& color, const Vector2f& center) {
		ParticleVertex v;
		for (int i = 0; i < 4; ++i) {
			v.x = pos.x;
			v.y = pos.y;
			v.z = 0.0f;
			v.uv = tex.getUV(i);
			v.scale.x = scaleX;
			v.scale.y = scaleY;
			v.dimension = tex.dim;
			v.rotationIndex.x = rotation;
			v.rotationIndex.y = i;
			v.color = color;
			spriteCtx->buffer->append(v);
		}
	}

	void draw(const Sprite& sprite) {
		draw(sprite.position, sprite.texture, sprite.rotation, sprite.scale.x, sprite.scale.y,sprite.color);
	}

	void draw(const BasicSprite& sprite) {
		draw(sprite.position, sprite.texture, sprite.rotation, sprite.scale.x, sprite.scale.y, sprite.color);
	}

	void drawTiledXY(const v2& position, const v2& size, const Texture& texture, float cornersize, const Color& color) {
		v2 center = position;
		center.x += size.x * 0.5f;
		center.y -= size.y * 0.5f;
		float ch = size.y - 2.0f * cornersize;
		float cw = size.x - 2.0f * cornersize;
		float hcz = cornersize * 0.5f;
		float sx = (size.x - 2.0f * cornersize) / (texture.dim.x - 2.0f * cornersize);
		float sy = (size.y - 2.0f * cornersize) / (texture.dim.y - 2.0f * cornersize);
		// left top corner 
		v2 p = center;
		ds::Texture tex = math::buildTexture(texture.rect.top, texture.rect.left, cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x - cw * 0.5f - hcz;
		p.y = center.y + ch * 0.5f + hcz;
		draw(p, tex, 0.0f, 1.0f, 1.0f, color);
		// right top corner
		tex = math::buildTexture(texture.rect.top, texture.rect.right - cornersize, cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x + cw * 0.5f + hcz;
		p.y = center.y + ch * 0.5f + hcz;
		draw(p, tex, 0.0f, 1.0f, 1.0f, color);
		// left bottom corner
		tex = math::buildTexture(texture.rect.bottom - cornersize, texture.rect.left, cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x - cw * 0.5f - hcz;
		p.y = center.y - ch * 0.5f - hcz;
		draw(p, tex, 0.0f, 1.0f, 1.0f, color);
		// right bottom corner
		tex = math::buildTexture(texture.rect.bottom - cornersize, texture.rect.right - cornersize, cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x + cw * 0.5f + hcz;
		p.y = center.y - ch * 0.5f - hcz;
		draw(p, tex, 0.0f, 1.0f, 1.0f, color);
		// top
		tex = math::buildTexture(texture.rect.top, texture.rect.left + cornersize, texture.rect.width() - 2.0f * cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x;
		p.y = center.y + ch * 0.5f + hcz;
		draw(p, tex, 0.0f, sx, 1.0f, color);
		// bottom
		tex = math::buildTexture(texture.rect.bottom - cornersize, texture.rect.left + cornersize, texture.rect.width() - 2.0f * cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x;
		p.y = center.y - ch * 0.5f - hcz;
		draw(p, tex, 0.0f, sx, 1.0f, color);
		// left
		tex = math::buildTexture(texture.rect.top + cornersize, texture.rect.left, cornersize, texture.rect.height() - 2.0f *cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x - cw * 0.5f - hcz;
		p.y = center.y;
		draw(p, tex, 0.0f, 1.0f, sy, color);
		// right
		tex = math::buildTexture(texture.rect.top + cornersize, texture.rect.right - cornersize, cornersize, texture.rect.height() - 2.0f *cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x + cw * 0.5f + hcz;
		p.y = center.y;
		draw(p, tex, 0.0f, 1.0f, sy, color);
		// center
		tex = math::buildTexture(texture.rect.top + cornersize, texture.rect.left + cornersize, texture.rect.width() - 2.0f * cornersize, texture.rect.height() - 2.0f * cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x;
		p.y = center.y;
		draw(p, tex, 0.0f, sx, sy, color);

	}

	void drawTiledX(const v2& position, float width, const Texture& texture, float cornersize, const Color& color) {
		v2 center = position;
		center.x += width * 0.5f;
		float cw = width - 2.0f * cornersize;
		float hcz = cornersize * 0.5f;
		float sx = (width - 2.0f * cornersize) / (texture.dim.x - 2.0f * cornersize);
		// left 
		v2 p = center;
		ds::Texture tex = ds::math::buildTexture(texture.rect.top, texture.rect.left, cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x - cw * 0.5f - hcz;
		draw(p, tex, 0.0f, 1.0f, 1.0f, color);
		// right
		tex = ds::math::buildTexture(texture.rect.top, texture.rect.right - cornersize, cornersize, cornersize, texture.textureSize.x, texture.textureSize.y);
		p.x = center.x + cw * 0.5f + hcz;
		draw(p, tex, 0.0f, 1.0f, 1.0f, color);
		// center
		tex = ds::math::buildTexture(texture.rect.top, texture.rect.left + cornersize, texture.rect.width() - 2.0f * cornersize, texture.rect.height(), texture.textureSize.x, texture.textureSize.y);
		p.x = center.x;
		p.y = center.y;
		draw(p, tex, 0.0f, sx, 1.0f, color);

	}

	void drawTemplate(const char* name) {
		Sprite sp;
		if (renderer::getSpriteTemplates()->get(name,&sp)) {
			draw(sp);
		}
	}

	void shutdown() {
		assert(spriteCtx != 0);
		delete spriteCtx->buffer;
		//gDefaultMemory->deallocate(spriteCtx->sprites);
		delete spriteCtx;
	}
}

}
