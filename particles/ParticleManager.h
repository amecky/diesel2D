#pragma once
#include "ParticleSystem.h"
#include "..\compiler\AssetCompiler.h"
#include "..\io\Serializer.h"

namespace ds {

class ParticleManager : public Serializer {

struct Index {
	uint32 id;
	uint32 index;
	char debugName[32];
};

typedef std::vector<ParticleSystem*> ParticleSystems;

public:
	ParticleManager();
	~ParticleManager();
	void start(uint32 id,const Vector2f& pos);
	void load(BinaryLoader* loader);
	void setRenderer(Renderer* renderer) {
		m_Renderer = renderer;
	}
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	void setTextureID(int textureID) {
		m_TextureID = textureID;
	}
	void setAssetCompiler(AssetCompiler* assertCompiler) {
		m_AssetCompiler = assertCompiler;
	}
	void update(float elapsed);
	void render();
	void debug();
private:
	Renderer* m_Renderer;
	AssetCompiler* m_AssetCompiler;
	ParticleSystems m_Systems;
	Index m_Indices[128];
	int m_Counter;
	int m_BlendState;
	int m_TextureID;
};

}

