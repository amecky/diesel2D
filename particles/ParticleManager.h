#pragma once
#include "ParticleSystem.h"
#include "..\renderer\graphics.h"
#include "..\ui\IMGUI.h"
#include "ParticleSystemFactory.h"
#include "..\renderer\BatchBuffer.h"

namespace ds {

const int MAX_PARTICLES = 16386;

struct ParticleSystemGroup {
	uint32 id;
	char name[20];
	Array<int> systems;
};

struct ParticleEvent;
	
class ParticleManager : public DataFile {

public:
	ParticleManager();
	~ParticleManager();
	void init(const Descriptor& desc);
	void start(uint32 id,const v2& pos);
	void startGroup(uint32 id, const v2& pos);
	void stop(uint32 id);
	void setBlendState(int blendState) {
		m_BlendState = blendState;
	}
	ParticleSystem* getParticleSystem(uint32 id) {
		assert(_systems[id] != 0);
		return _systems[id];
	}
	void update(float elapsed);
	void render();

	ParticleSystem* create(int id, const char* name);
	ParticleSystem* create(const char* name);
	void removeSystem(int id);
	void debug();
	void fillModel(gui::ComponentModel<int>* model);
	bool saveData(JSONWriter& writer);
	const char* getFileName() const {
		return "particles\\particlesystems.json";
	}
	const ParticleSystemFactory& getFactory() const {
		return _factory;
	}
	virtual bool loadData(const JSONReader& loader);
	bool hasEvents() const {
		return _events.size() > 0;
	}
	int getEventsCount() const {
		return _events.size();
	}
	const ParticleEvent& getEvent(int index) {
		return _events[index];
	}
private:
	int findGroup(uint32 id);
	ParticleSystem** _systems;
	int m_BlendState;
	BatchBuffer<ParticleVertex>* _particles;
	ParticleSystemFactory _factory;
	Array<ParticleSystemGroup> _groups;
	Array<ParticleEvent> _events;
};

}
