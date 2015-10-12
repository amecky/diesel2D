#include "SpriteBatch.h"
#include "..\renderer\vertex_types.h"
#include "..\utils\Profiler.h"
#include "..\renderer\shader.h"
#include "..\sprites\Sprite.h"
#include "..\utils\Log.h"
#include "..\math\GameMath.h"
#include "..\renderer\VertexDeclaration.h"
#include "..\renderer\graphics.h"

namespace ds {

const uint32 QUAD_SIZE = 144;
const uint32 INDEX_SIZE = 4;
const uint32 PTC_SIZE = 36;
const uint32 MAX_QUADS = 2048;
const uint32 MAX_VERTICES = 4 * MAX_QUADS;
const uint32 MAX_INDICES = 6 * MAX_QUADS;

const float VP_ARRAY[] = {
	-0.5f,0.5f, 0.5f,0.5f, 
	0.5f,-0.5f, -0.5f,-0.5f
};

// ------------------------------------------------------------
//
// ------------------------------------------------------------
namespace sprites {

	const int MAX_SPRITES = 8192;

	struct SpriteBatchContext {

		int maxSprites;
		int bufferIndex;
		PTCVertex sprites[MAX_SPRITES * 4];
		int size;
		int index;
		int maxVertices;
		int descriptorID;
		BitmapFont* font;

		SpriteBatchContext() : size(0) , font(0) {}

		~SpriteBatchContext() {}
	};

	SpriteBatchContext spriteCtx;

	void init() {		
		spriteCtx.size = 0;
		spriteCtx.index = 0;
		spriteCtx.maxVertices = MAX_SPRITES * 4;
		Descriptor desc;
		desc.blendState = renderer::getDefaultBlendState();
		desc.shader = renderer::getDefaultShaderID();
		desc.texture = 0;
		spriteCtx.descriptorID = renderer::addDescriptor(desc);
		spriteCtx.bufferIndex = renderer::createVertexBuffer(VD_PTC,MAX_SPRITES * 4);
	}

	void initializeTextSystem(int textureID, const char* fontName) {
		spriteCtx.font = renderer::loadBitmapFont(fontName, textureID);		
	}

	int getDescriptorID() {
		return spriteCtx.descriptorID;
	}

	void setDescriptorID(int id) {
		spriteCtx.descriptorID = id;
		flush();
	}

	void begin() {
		spriteCtx.size = 0;
		spriteCtx.index = 0;
	}

	int getCurrentTextureID() {
		return renderer::getDescriptorData()->textures[spriteCtx.descriptorID];
	}

	void setTexture(int textureID) {
		flush();		
		renderer::getDescriptorData()->textures[spriteCtx.descriptorID] = textureID;
	}

	void setShaderID(int shader) {
		// the shader in desctiptor
		renderer::getDescriptorData()->shaders[spriteCtx.descriptorID] = shader;
		//flush();
	}

	void end() {
		if (spriteCtx.size > 0) {
			PR_START("sprites")
			renderer::fillBuffer(spriteCtx.bufferIndex, spriteCtx.sprites, spriteCtx.index);
			renderer::draw(spriteCtx.descriptorID, spriteCtx.bufferIndex, spriteCtx.index, renderer::getQuadIndexBufferIndex());
			renderer::drawCounter().sprites += spriteCtx.index;
			PR_END("sprites")
		}
	}

	void flush() {
		end();
		begin();
	}

	void drawText(int x, int y, const char* text, float scaleX, float scaleY, const Color& color) {
		assert(spriteCtx.font != 0);
		float padding = 0.0f;
		int len = strlen(text);
		for (int cnt = 0; cnt < len; ++cnt) {
			char c = text[cnt];
			if (spriteCtx.font->contains(c)) {
				CharDef cd = spriteCtx.font->getCharDef(c);
				padding = (cd.width + 2)  * scaleX;
				float dimX = cd.width * scaleX;
				float dimY = spriteCtx.font->getCharHeight() * scaleY;
				draw(Vector2f(x + dimX * 0.5f, y + dimY * 0.5f), math::buildTexture(cd.texureRect), 0.0f, scaleX, scaleY, color);
				x += dimX + 4;
			}
		}
	}
	
	void drawText(BitmapFont* font,int x, int y, const char* text,int padding, float scaleX, float scaleY, const Color& color) {
		assert(font != 0);
		int len = strlen(text);
		for (int cnt = 0; cnt < len; ++cnt) {
			char c = text[cnt];
			if (font->contains(c)) {
				CharDef cd = font->getCharDef(c);
				//padding = (cd.width + 2)  * scaleX;
				float dimX = cd.width * scaleX;
				float dimY = font->getCharHeight() * scaleY;
				draw(v2(x + dimX * 0.5f, y + dimY * 0.5f), math::buildTexture(cd.texureRect,font->getTextureSize(),font->getTextureSize(),false), 0.0f, scaleX, scaleY, color);
				x += dimX + padding;
			}
		}
	}

	void draw(const Vector2f& pos, int textureID, const Vector4f& uv, const Vector2f& dim, float rotation, float scaleX, float scaleY, const Color& color, const Vector2f& center) {
		int vertexCount = spriteCtx.index;
		if ((vertexCount + 4) >= spriteCtx.maxVertices) {
			flush();
		}
		int idx = spriteCtx.index;
		spriteCtx.sprites[idx].uv.x = uv.x;
		spriteCtx.sprites[idx].uv.y = uv.y;
		spriteCtx.sprites[idx+1].uv.x = uv.z;
		spriteCtx.sprites[idx+1].uv.y = uv.y;
		spriteCtx.sprites[idx+2].uv.x = uv.z;
		spriteCtx.sprites[idx+2].uv.y = uv.w;
		spriteCtx.sprites[idx+3].uv.x = uv.x;
		spriteCtx.sprites[idx+3].uv.y = uv.w;

		Vector2f cor = pos;
		cor = cor - ds::renderer::getSelectedViewport().getPosition();
		Vector2f p(0, 0);
		for (int i = 0; i < 4; ++i) {
			p.x = VP_ARRAY[i * 2] * dim.x;
			p.y = VP_ARRAY[i * 2 + 1] * dim.y;
			p = p - center;
			Vector2f np = vector::srt(cor, p, scaleX, scaleY, rotation);
			spriteCtx.sprites[idx + i].x = np.x;
			spriteCtx.sprites[idx + i].y = np.y;
			spriteCtx.sprites[idx + i].z = 0.0f;
			spriteCtx.sprites[idx + i].color = color;
		}	
		spriteCtx.index += 4;
		++spriteCtx.size;
	}

	void draw(const Texture& texture, const ParticleArray& array) {
		int idx = spriteCtx.index;
		for (uint32 i = 0; i < array.countAlive; ++i) {
			if ((spriteCtx.index + 4) >= spriteCtx.maxVertices) {
				flush();
			}			
			idx = spriteCtx.index;
			for (int j = 0; j < 4; ++j) {
				spriteCtx.sprites[idx + j].x = array.vertices[i * 4 + j].x;
				spriteCtx.sprites[idx + j].y = array.vertices[i * 4 + j].y;
				spriteCtx.sprites[idx + j].z = 0.0f;
				spriteCtx.sprites[idx + j].uv = array.vertices[i * 4 + j].uv;
				spriteCtx.sprites[idx + j].color = array.color[i];
			}
			spriteCtx.index += 4;
			++spriteCtx.size;
		}
	}

	void draw(const Shape& shape, const Texture& tex) {
		int vertexCount = spriteCtx.index;
		if ((vertexCount + 4) >= spriteCtx.maxVertices) {
			renderer::setTexture(tex.textureID);
			flush();
		}
		int idx = spriteCtx.index;
		spriteCtx.sprites[idx].uv.x = tex.uv.x;
		spriteCtx.sprites[idx].uv.y = tex.uv.y;
		spriteCtx.sprites[idx + 1].uv.x = tex.uv.z;
		spriteCtx.sprites[idx + 1].uv.y = tex.uv.y;
		spriteCtx.sprites[idx + 2].uv.x = tex.uv.z;
		spriteCtx.sprites[idx + 2].uv.y = tex.uv.w;
		spriteCtx.sprites[idx + 3].uv.x = tex.uv.x;
		spriteCtx.sprites[idx + 3].uv.y = tex.uv.w;
		//Vector2f cor = pos;
		//cor = cor - ds::renderer::getSelectedViewport().getPosition();
		//Vector2f p(0, 0);
		for (int i = 0; i < 4; ++i) {
			//p.x = VP_ARRAY[i * 2] * tex.dim.x;
			//p.y = VP_ARRAY[i * 2 + 1] * tex.dim.y;
			//p = p - center;
			//Vector2f np = vector::srt(cor, p, scaleX, scaleY, rotation);
			Vector2f np = shape.v[i] - ds::renderer::getSelectedViewport().getPosition();
			spriteCtx.sprites[idx + i].x = np.x;
			spriteCtx.sprites[idx + i].y = np.y;
			spriteCtx.sprites[idx + i].z = 0.0f;
			spriteCtx.sprites[idx + i].color = shape.color;
		}
		spriteCtx.index += 4;
		++spriteCtx.size;
	}

	void draw(const Vector2f& pos, const Texture& tex, float rotation, float scaleX, float scaleY, const Color& color, const Vector2f& center) {
		int vertexCount = spriteCtx.index;
		if ((vertexCount + 4) >= spriteCtx.maxVertices  ) {
			renderer::setTexture(tex.textureID);
			flush();
		}
		int idx = spriteCtx.index;
		spriteCtx.sprites[idx].uv.x = tex.uv.x;
		spriteCtx.sprites[idx].uv.y = tex.uv.y;
		spriteCtx.sprites[idx + 1].uv.x = tex.uv.z;
		spriteCtx.sprites[idx + 1].uv.y = tex.uv.y;
		spriteCtx.sprites[idx + 2].uv.x = tex.uv.z;
		spriteCtx.sprites[idx + 2].uv.y = tex.uv.w;
		spriteCtx.sprites[idx + 3].uv.x = tex.uv.x;
		spriteCtx.sprites[idx + 3].uv.y = tex.uv.w;
		Vector2f cor = pos;
		cor = cor - ds::renderer::getSelectedViewport().getPosition();
		Vector2f p(0, 0);
		for (int i = 0; i < 4; ++i) {
			p.x = VP_ARRAY[i * 2] * tex.dim.x;
			p.y = VP_ARRAY[i * 2 + 1] * tex.dim.y;
			p = p - center;
			Vector2f np = vector::srt(cor, p, scaleX, scaleY, rotation);
			spriteCtx.sprites[idx + i].x = np.x;
			spriteCtx.sprites[idx + i].y = np.y;
			spriteCtx.sprites[idx + i].z = 0.0f;
			spriteCtx.sprites[idx + i].color = color;
		}
		spriteCtx.index += 4;
		++spriteCtx.size;
	}

	void draw(const Sprite& sprite) {
		draw(sprite.position, sprite.texture, sprite.rotation, sprite.scale.x, sprite.scale.y,sprite.color);
	}

	void draw(const DataContainer& container,const Texture& texture,int posIdx) {
		const Vector2f* p = container.getChannel<Vector2f>(posIdx);
		for ( int i = 0; i < container.size(); ++i ) {
			draw(*p,texture);
			++p;
		}
	}

	void draw(const DataContainer& container,const Texture& texture,int posIdx,int angleIdx) {
		const Vector2f* p = container.getChannel<Vector2f>(posIdx);
		const float* a = container.getChannel<float>(angleIdx);
		for ( int i = 0; i < container.size(); ++i ) {
			draw(*p,texture,*a);
			++p;
			++a;
		}
	}

	void draw(const DataContainer& container,const Texture& texture,int posIdx,int angleIdx,int scaleIdx) {
		const Vector2f* p = container.getChannel<Vector2f>(posIdx);
		const float* a = container.getChannel<float>(angleIdx);
		const Vector2f* s = container.getChannel<Vector2f>(scaleIdx);
		for ( int i = 0; i < container.size(); ++i ) {
			draw(*p,texture,*a,s->x,s->y);
			++p;
			++a;
			++s;
		}
	}

	void drawTemplate(const char* name) {
		Sprite sp;
		if (renderer::getSpriteTemplates()->get(name,&sp)) {
			draw(sp);
		}
	}
}

}
