#pragma once
#include "ParticleSystem.h"
#include "..\compiler\AssetCompiler.h"
#include "..\io\Serializer.h"

namespace ds {
	
class ParticleManager : public Serializer {

typedef std::vector<NewParticleSystem*> ParticleSystems;

public:
	ParticleManager();
	~ParticleManager();
	void init(const Descriptor& desc) {
		bufferIndex = renderer::createVertexBuffer(VD_PARTICLE, 4096 * 4, true);
		indexBufferIndex = renderer::getQuadIndexBufferIndex();
		descriptorIndex = renderer::addDescriptor(desc);
	}
	void start(uint32 id,const Vector2f& pos);
	void stop(uint32 id);
	void load(BinaryLoader* loader);
	void setRenderer(Renderer* renderer) {
		m_Renderer = renderer;
	}
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	void setAssetCompiler(AssetCompiler* assertCompiler) {
		m_AssetCompiler = assertCompiler;
	}
	NewParticleSystem* getParticleSystem(uint32 id) {
		return m_Systems[m_Index[id]];
	}
	void update(float elapsed);
	void render();
	void debug();
private:
	void end();
	void flush();
	void begin();
	Renderer* m_Renderer;
	AssetCompiler* m_AssetCompiler;
	ParticleSystems m_Systems;
	int m_BlendState;
	int m_Index[512];

	int bufferIndex;
	int indexBufferIndex;
	int descriptorIndex;
	ParticleVertex particles[4096 * 4];
	int m_ParticleIndex;
	//int m_TextureID;
	//int m_ShaderID;
};

}
