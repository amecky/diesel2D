#pragma once
#include "ParticleSystem.h"
#include "..\compiler\AssetCompiler.h"
#include "..\io\Serializer.h"
#include "..\renderer\graphics.h"
#include "..\ui\IMGUI.h"
#include "ParticleSystemFactory.h"

namespace ds {
	
class ParticleManager : public DataFile {


typedef std::vector<NewParticleSystem*> ParticleSystems;

public:
	ParticleManager();
	~ParticleManager();
	void init(const Descriptor& desc);
	//void start(uint32 id, const ParticleGeneratorData& data);
	void start(uint32 id,const Vector3f& pos);
	void stop(uint32 id);
	//void load(BinaryLoader* loader);
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	NewParticleSystem* getParticleSystem(uint32 id) {
		return m_Systems[m_Index[id]];
	}
	void update(float elapsed);
	void render();

	NewParticleSystem* create(int id, const char* name);

	void debug();
	void fillModel(gui::ComponentModel<int>* model);
	bool exportData(JSONWriter& writer);
	bool importData(JSONReader& reader);
	bool saveData(BinaryWriter& writer);
	bool loadData(BinaryLoader& loader);
	const char* getJSONFileName() const {
		return "particles\\particlesystems.json";
	}
	const char* getFileName() const {
		return "particlesystems";
	}
	const ParticleSystemFactory& getFactory() const {
		return _factory;
	}
private:
	void end();
	void flush();
	void begin();
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
	ParticleSystemFactory _factory;

	
};

}
