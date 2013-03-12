#pragma once
#include "..\nodes\QuadMeshNode.h"
#include "ParticleEmitter.h"
#include "ParticleBuffer.h"
#include "ParticleAffector.h"
#include "..\renderer\Renderer.h"
#include "..\renderer\render_types.h"

namespace ds {

class ParticleEmitterNode : public ds::Node {

typedef std::vector<ParticleEmitter*> EmitterList;
typedef std::vector<ParticleAffector*> AffectorList;

public:
	ParticleEmitterNode(Renderer* renderer,int material,uint32 maxQuads);
	virtual ~ParticleEmitterNode() {}
	void update(float elapsed);
	uint32 addEmitter(ParticleEmitter* emitter);
	void removeEmitter(uint32 id);
	void prepareRendering();
	void postRendering();
	void draw();
	void emitParticles();
	void addParticleAffector(ParticleAffector* particleAffector);
private:
	EmitterList m_EmitterList;
	ParticleBuffer* m_ParticleBuffer;
	AffectorList m_AffectorList;
	uint32 m_IdCounter;
};

}
