#pragma once
#include "ParticleEmitter.h"
#include "ParticleBuffer.h"
#include "ParticleAffector.h"
#include "..\renderer\Renderer.h"
#include "..\renderer\render_types.h"
#include "..\nodes\SpriteBatch.h"
#include "..\renderer\Camera2D.h"
//#include "..\renderer\DebugRenderer.h"

namespace ds {

class ParticleSystem {

public:
	ParticleSystem(Renderer* renderer,uint32 maxQuads,int textureID,int blendState = -1);
	virtual ~ParticleSystem() {
		LOG(logINFO) << "Destructing particles system";
		delete m_ColorPath;
		if ( m_RotationPath != 0 ) {
			delete m_RotationPath;
		}
		if ( m_SizePath != 0 ) {
			delete m_SizePath;
		}
		if ( m_RadialVelocityPath != 0 ) {
			delete m_RadialVelocityPath;
		}
		delete m_ParticleData;
		delete m_Emitter;
		delete m_SpriteBatch;
		delete m_ParticleBuffer;
	}
	void update(float elapsed);
	void setEmitter(ParticleEmitter* emitter);
	void draw();
	void emitParticles();
	void setParticleData(ParticleData* particleData);
	ParticleData* getParticleData() const {
		return m_ParticleData;
	}
	template<class PE>
	PE* getEmitter() {
		return static_cast<PE*>(m_Emitter);
	}
	void setEmitterPosition(const Vec2& pos);
	const uint32 numParticles() const {
		return m_ParticleBuffer->size();
	}
	const bool isActive() const {
		return numParticles() > 0;
	}
	void resetEmitterCounter();
	void setCamera(Camera2D* camera) {
		m_Camera = camera;
	}
	void start(const Vec2& startPos);
	void stop();
	ColorPath* getColorPath() {
		return m_ColorPath;
	}
	Vec2Path* createSizePath() {
		if ( m_SizePath == 0 ) {
			m_SizePath = new Vec2Path;
		}
		else {
			m_SizePath->reset();
		}
		return m_SizePath;
	}
	FloatPath* createRadialVelocityPath() {
		if ( m_RadialVelocityPath == 0 ) {
			m_RadialVelocityPath = new FloatPath;
		}
		else {
			m_RadialVelocityPath->reset();
		}
		return m_RadialVelocityPath;
	}
	FloatPath* createRotationPath() {
		if ( m_RotationPath == 0 ) {
			m_RotationPath = new FloatPath;
		}
		else {
			m_RotationPath->reset();
		}
		return m_RotationPath;
	}
	const uint32 getMaxParticles() const {
		return m_ParticleBuffer->getMaxParticles();
	}
private:
	int m_TextureID;
	Camera2D* m_Camera;
	ParticleData* m_ParticleData;
	SpriteBatch* m_SpriteBatch;
	ParticleEmitter* m_Emitter;
	ParticleBuffer* m_ParticleBuffer;
	ColorPath* m_ColorPath;
	Vec2Path* m_SizePath;
	FloatPath* m_RadialVelocityPath;
	FloatPath* m_RotationPath;
	uint32 m_IdCounter;
};


}
