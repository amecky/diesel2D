#pragma once
#include "ParticleSystem.h"
#include "..\renderer\graphics.h"
#include "..\ui\IMGUI.h"
#include "ParticleSystemFactory.h"

namespace ds {
	
class ParticleManager : public DataFile {

public:
	ParticleManager();
	~ParticleManager();
	void init(const Descriptor& desc);
	void start(uint32 id,const Vector3f& pos);
	void stop(uint32 id);
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	NewParticleSystem* getParticleSystem(uint32 id) {
		assert(_systems[id] != 0);
		return _systems[id];
	}
	void update(float elapsed);
	void render();

	NewParticleSystem* create(int id, const char* name);
	NewParticleSystem* create(const char* name);
	void removeSystem(int id);
	void debug();
	void fillModel(gui::ComponentModel<int>* model);
	bool saveData(JSONWriter& writer);
	bool loadData(JSONReader& reader);
	const char* getFileName() const {
		return "particles\\particlesystems.json";
	}
	const ParticleSystemFactory& getFactory() const {
		return _factory;
	}
private:
	void end();
	void flush();
	void begin();
	NewParticleSystem** _systems;
	int m_BlendState;
	int bufferIndex;
	int indexBufferIndex;
	int descriptorIndex;
	ParticleVertex particles[4096 * 4];
	int m_ParticleIndex;
	ParticleSystemFactory _factory;
};

}
