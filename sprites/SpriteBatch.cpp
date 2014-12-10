#include "SpriteBatch.h"
#include "..\renderer\vertex_types.h"
#include "..\utils\Profiler.h"
#include "..\renderer\shader.h"
#include "..\sprites\Sprite.h"
#include "..\renderer\Renderer.h"
#include "..\utils\Log.h"
#include "..\math\GameMath.h"
#include "..\renderer\VertexDeclaration.h"

namespace ds {

const uint32 QUAD_SIZE = 144;
const uint32 INDEX_SIZE = 4;
const uint32 PTC_SIZE = 36;
const uint32 MAX_QUADS = 2048;
const uint32 MAX_VERTICES = 4 * MAX_QUADS;
const uint32 MAX_INDICES = 6 * MAX_QUADS;

struct BatchContext {

	int handle;
	int currentTextureID;
	int currentBlendState;
	int shaderID;
	uint32 vertexCounter;
	Renderer* renderer;
};

static BatchContext batch_context;

const float VP_ARRAY[] = {
	-0.5f,0.5f, 0.5f,0.5f, 
	0.5f,-0.5f, -0.5f,-0.5f
};

namespace gfx {

	void init(Renderer* renderer, int shaderID, int blendStateID) {
		batch_context.handle = renderer->createBufferHandle(PT_TRI, VD_PTC, GBT_BOTH, true);
		batch_context.renderer = renderer;
		batch_context.vertexCounter = 0;
		batch_context.currentTextureID = -1;
		batch_context.shaderID = shaderID;
		batch_context.currentBlendState = blendStateID;
	}

	void endBatch() {
		if (batch_context.vertexCounter > 0) {
			PRS("gfx::endBatch")
			batch_context.renderer->setCurrentShader(batch_context.shaderID);
			batch_context.renderer->setTexture(batch_context.currentTextureID, 0);
			batch_context.renderer->setTexture(batch_context.shaderID, "gTex", batch_context.currentTextureID);
			batch_context.renderer->drawBuffer(batch_context.handle, batch_context.currentTextureID);
			batch_context.renderer->resetBuffer(batch_context.handle);
			renderContext->drawCounter.incFlushes();
			batch_context.vertexCounter = 0;
			PRE("gfx::endBatch")
		}
	}

	void flushBatch() {
		endBatch();
		batch_context.vertexCounter = 0;
	}

	void draw(int textureID, const NParticleArray& particleArray) {
		PRS("gfx::draw")
		if (particleArray.size() > 0) {
			PRS("gfx::draw::prepare")
			WORD* ib;
			PTCVertex* vb;
			int vc = particleArray.size() * 4;
			int ic = particleArray.size() * 6;
			batch_context.renderer->lockBuffer(batch_context.handle, vc, ic, (float**)&vb, (void**)&ib);
			Vector4f* pos = particleArray.begin(0);
			Vector4f* scale = particleArray.begin(2);
			Vector4f* color = particleArray.begin(5);
			Vector4f* rot = particleArray.begin(3);
			Vector4f* uv = particleArray.begin(6);
			Vector4f* dim = particleArray.begin(7);
			Vector4f uvar[4];
			PRS("gfx::draw::prepare::VB")
			for (uint32 n = 0; n <particleArray.size(); ++n) {
				if ((batch_context.vertexCounter + 4) >= MAX_VERTICES || textureID != batch_context.currentTextureID) {
					batch_context.currentTextureID = textureID;
					flushBatch();
				}

				uvar[0].x = uv->x;
				uvar[0].y = uv->y;
				uvar[1].x = uv->z;
				uvar[1].y = uv->y;
				uvar[2].x = uv->z;
				uvar[2].y = uv->w;
				uvar[3].x = uv->x;
				uvar[3].y = uv->w;

				Vector2f cor;
				cor.x = pos->x;
				cor.y = pos->y;
				cor = cor - ds::renderer::getSelectedViewport().getPosition();
				Vector2f p(0, 0);
				for (int i = 0; i < 4; ++i) {
					p.x = VP_ARRAY[i * 2] * dim->x;
					p.y = VP_ARRAY[i * 2 + 1] * dim->y;
					//p = p - center;
					Vector2f np = vector::srt(cor, p, scale->x, scale->y, rot->x);					
					vb->x = np.x;
					vb->y = np.y;
					vb->z = 0.0f;
					vb->uv.x = uvar[i].x;
					vb->uv.y = uvar[i].y;
					vb->color.r = color->x;
					vb->color.g = color->y;
					vb->color.b = color->z;
					vb->color.a = color->w;
					++vb;
				}
				++pos;
				++scale;
				++rot;
				++color;
				++uv;
				++dim;
				batch_context.vertexCounter += 4;
			}
			PRE("gfx::draw::prepare::VB")
			PRS("gfx::draw::prepare::IB")
			uint32 indexCounter = batch_context.vertexCounter / 4;// *6;
			for (uint32 i = 0; i < indexCounter; ++i) {
				*ib = i * 4 + 0;
				++ib;
				*ib = i * 4 + 1;
				++ib;
				*ib = i * 4 + 2;
				++ib;
				*ib = i * 4 + 2;
				++ib;
				*ib = i * 4 + 3;
				++ib;
				*ib = i * 4 + 0;
				++ib;
			}
			PRE("gfx::draw::prepare::IB")
			batch_context.renderer->unlockBuffer(batch_context.handle);
			PRE("gfx::draw::prepare")
			flushBatch();
			renderContext->drawCounter.addSprites(particleArray.size());
			
		}	
		PRE("gfx::draw")
	}

	
	
}
// ------------------------------------------------------------
//
// ------------------------------------------------------------
namespace sprites {

	const int MAX_SPRITES = 8192;

	struct SpriteBatchContext {

		Renderer* renderer;
		int maxSprites;
		int shaderID;
		int blendStateID;
		LPDIRECT3DINDEXBUFFER9 indexBuffer;
		LPDIRECT3DVERTEXBUFFER9 vertexBuffer;
		PTCVertex sprites[MAX_SPRITES * 4];
		int size;
		int index;
		int maxVertices;
		int textureID;
		int vertexSize;

		SpriteBatchContext() : size(0) , vertexBuffer(0) , indexBuffer(0) {}

		~SpriteBatchContext() {
			if (vertexBuffer) {
				SAFE_RELEASE(vertexBuffer);
			}
			if (indexBuffer) {
				SAFE_RELEASE(indexBuffer);
			}
		}
	};

	SpriteBatchContext spriteCtx;

	void init(Renderer* renderer) {		
		spriteCtx.size = 0;
		spriteCtx.index = 0;
		spriteCtx.maxVertices = MAX_SPRITES * 4;
		spriteCtx.renderer = renderer;
		spriteCtx.blendStateID = renderer->getDefaultBlendState();
		spriteCtx.shaderID = renderer->getDefaultShaderID();
		VDStruct vd = renderer::getVertexDeclaration(VD_PTC);
		spriteCtx.vertexSize = vd.vertexSize;
		D3DPOOL pool = D3DPOOL_DEFAULT;
		DWORD usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
		int size = MAX_SPRITES * 4;
		LOGC("sprites") << "creating new vertex buffer - size: " << size << " vd size: " << spriteCtx.vertexSize;
		HR(renderContext->device->CreateVertexBuffer(size * spriteCtx.vertexSize, usage, 0, pool, &spriteCtx.vertexBuffer, NULL));

		int indices = MAX_SPRITES * 6;
		pool = D3DPOOL_MANAGED;
		usage = D3DUSAGE_WRITEONLY;
		HR(renderContext->device->CreateIndexBuffer(indices * sizeof(WORD), usage, D3DFMT_INDEX16, pool, &spriteCtx.indexBuffer, NULL));
		LOGC("sprites") << "new IndexBuffer created size: " << indices;
		WORD* indexBuffer;
		HR(spriteCtx.indexBuffer->Lock(0, indices * sizeof(WORD), (void**)&indexBuffer, 0));

		for (uint32 i = 0; i < MAX_SPRITES; ++i) {
			*indexBuffer = i * 4 + 0;
			++indexBuffer;
			*indexBuffer = i * 4 + 1;
			++indexBuffer;
			*indexBuffer = i * 4 + 2;
			++indexBuffer;
			*indexBuffer = i * 4 + 2;
			++indexBuffer;
			*indexBuffer = i * 4 + 3;
			++indexBuffer;
			*indexBuffer = i * 4 + 0;
			++indexBuffer;
		}
		HR(spriteCtx.indexBuffer->Unlock());
		
	}

	void begin() {
		spriteCtx.size = 0;
		spriteCtx.index = 0;
	}

	void setShaderID(int shaderID) {
		if (spriteCtx.shaderID != shaderID) {
			sprites::flush();
		}
		spriteCtx.shaderID = shaderID;
	}

	// -------------------------------------------------------
	// Prepare buffer
	// -------------------------------------------------------
	void prepareBuffer() {
		PRS("sprites::prepare")
		assert(spriteCtx.vertexBuffer != 0);
		if (spriteCtx.size  > 0) {
			char* vertexBuffer;
			int vc = spriteCtx.size * 4;
			DWORD flag = D3DLOCK_DISCARD;
			HR(spriteCtx.vertexBuffer->Lock(0, vc * spriteCtx.vertexSize, (void**)&vertexBuffer, flag));
			memcpy(vertexBuffer, spriteCtx.sprites, spriteCtx.index * spriteCtx.vertexSize);
			HR(spriteCtx.vertexBuffer->Unlock());
		}
		PRE("sprites::prepare")
	}

	void end() {
		if (spriteCtx.size > 0) {
			PR_START("sprites")
			prepareBuffer();
			spriteCtx.renderer->setCurrentShader(spriteCtx.shaderID);
			spriteCtx.renderer->setTexture(spriteCtx.textureID, 0);
			spriteCtx.renderer->setTexture(spriteCtx.shaderID, "gTex", spriteCtx.textureID);
			//m_Renderer->setBlendState(m_CurrentBlendState);
			PR_START("sprites::drawBuffer")
				//if (m_CurrentVD != vDef) {
				//m_CurrentVD = vDef;
			HR(renderContext->device->SetVertexDeclaration(renderContext->vdStructs[VD_PTC].declaration->get()););
			//}
			HR(renderContext->device->SetIndices(spriteCtx.indexBuffer));
			HR(renderContext->device->SetStreamSource(0, spriteCtx.vertexBuffer, 0, spriteCtx.vertexSize));

			D3DPRIMITIVETYPE pt = D3DPT_TRIANGLELIST;
			int numPrimitives = spriteCtx.size * 2;
			renderContext->drawCounter.addDrawCall();
			renderContext->drawCounter.addPrimitives(numPrimitives);
			renderContext->drawCounter.addIndices(numPrimitives * 3);
			Shader* shader = &renderer::getShader(spriteCtx.shaderID);
			renderContext->drawCounter.addShader();
			uint32 numPasses = spriteCtx.renderer->startShader(shader);
			for (UINT p = 0; p < numPasses; ++p) {
				HR(shader->m_FX->BeginPass(p));
				spriteCtx.renderer->setShaderParameter(shader);
				HR(renderContext->device->DrawIndexedPrimitive(pt, 0, 0, spriteCtx.size * 4, 0, numPrimitives));
				HR(shader->m_FX->EndPass());
			}
			spriteCtx.renderer->endShader(shader);
			PR_END("sprites::drawBuffer")
			renderContext->drawCounter.incFlushes();
			renderContext->drawCounter.addSprites(spriteCtx.size);
			PR_END("sprites")
		}
	}

	void flush() {
		end();
		begin();
	}

	void draw(const Vector2f& pos, int textureID, const Vector4f& uv, const Vector2f& dim, float rotation, float scaleX, float scaleY, const Color& color, const Vector2f& center) {
		int vertexCount = spriteCtx.index;
		if ((vertexCount + 4) >= spriteCtx.maxVertices || textureID != spriteCtx.textureID) {
			spriteCtx.textureID = textureID;
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
		for (uint32 i = 0; i < array.countAlive; ++i) {
			draw(array.position[i], texture, array.rotation[i], array.scale[i].x, array.scale[i].y, array.color[i]);			
		}
	}

	void draw(const Vector2f& pos, const Texture& tex, float rotation, float scaleX, float scaleY, const Color& color, const Vector2f& center) {
		int vertexCount = spriteCtx.index;
		if ((vertexCount + 4) >= spriteCtx.maxVertices || tex.textureID != spriteCtx.textureID) {
			spriteCtx.textureID = tex.textureID;
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
		draw(sprite.getPosition(), sprite.getTextureID(), sprite.getUV(), sprite.getDimension(), sprite.getAngle(), sprite.getScale().x, sprite.getScale().y,sprite.getColor());
	}

	void draw(const MySprite& sprite) {
		draw(sprite.position, sprite.texture.textureID, sprite.texture.uv, sprite.texture.dim, sprite.angle, sprite.scale.x, sprite.scale.y, sprite.color);
	}


	void create(MySprite* sprite, int layer, const Vector2f& pos, const Rect& r, float rotation, const Vector2f& scale, const Color& clr) {
		sprite->layer = layer;
		sprite->position = pos;
		sprite->texture.uv = math::getTextureCoordinates(r);
		sprite->texture.textureID = 0;
		sprite->texture.dim = Vector2f(r.width(), r.height());
		sprite->angle = rotation;
		sprite->color = clr;
		sprite->timer = 0.0f;
		sprite->scale = scale;
		sprite->target = Vector2f(0, 0);
		sprite->index = 0;
	}
}

// ------------------------------------------------------------
//
// ------------------------------------------------------------
SpriteBatch::SpriteBatch(Renderer* renderer) {
}


SpriteBatch::~SpriteBatch() {
	LOGC("SpriteBatch") << "destructing SpriteBatch";	
}

void SpriteBatch::draw(int textureID, const Rect& textureRect, const ParticleArray& particleArray) {
	/*
	float u1, v1, u2, v2;
	ds::math::getTextureCoordinates(textureRect, 1024, 1024, &u1, &v1, &u2, &v2, true);
	int cnt = 0;
	float dimX = textureRect.width();
	float dimY = textureRect.height();
	for (uint32 n = 0; n <particleArray.countAlive; ++n) {
		if ((m_VertexCounter + 4) >= m_MaxVertices || textureID != m_CurrentTextureID) {
			m_CurrentTextureID = textureID;
			flush();
		}

		char* buffer = m_DataBuffer + QUAD_SIZE * m_Index;
		(*(SpritePlane*)buffer).v[0].uv.x = u1;
		(*(SpritePlane*)buffer).v[0].uv.y = v1;
		(*(SpritePlane*)buffer).v[1].uv.x = u2;
		(*(SpritePlane*)buffer).v[1].uv.y = v1;
		(*(SpritePlane*)buffer).v[2].uv.x = u2;
		(*(SpritePlane*)buffer).v[2].uv.y = v2;
		(*(SpritePlane*)buffer).v[3].uv.x = u1;
		(*(SpritePlane*)buffer).v[3].uv.y = v2;

		Vector2f cor;
		cor = particleArray.position[n];
		//cor.y = particleArray.data[cnt + 1];
		cor = cor - m_Renderer->getViewport()->getPosition();
		Vector2f p(0, 0);
		for (int i = 0; i < 4; ++i) {
			p.x = VP_ARRAY[i * 2] * dimX;
			p.y = VP_ARRAY[i * 2 + 1] * dimY;
			//p = p - center;
			Vector2f np = vector::srt(cor, p, particleArray.scale[n].x, particleArray.scale[n].y, particleArray.rotation[n]);
			(*(SpritePlane*)buffer).v[i].x = np.x;
			(*(SpritePlane*)buffer).v[i].y = np.y;
			(*(SpritePlane*)buffer).v[i].z = 0.0f;
			(*(SpritePlane*)buffer).v[i].color = particleArray.color[n];
		}

		cnt += SIMPLE_PARTICLE_SIZE;
		++m_Index;
		m_VertexCounter += 4;
	}
	*/
}

}
