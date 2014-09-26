#pragma once
#include "ParticleSystem.h"
#include "EmitterMesh.h"
#include "..\lib\container\List.h"
#include "..\io\Serializer.h"
#include <IPath.h>
#include "..\base\GameObject.h"
#include "..\data\Gizmo.h"
#include "Particle.h"
#include "..\renderer\Camera2D.h"
#include "DynamicSettings.h"
#include "ParticleModifier.h"
#include "ParticleEmitter.h"
#include "..\utils\Profiler.h"
#include "..\script\ASM.h"

namespace ds {
	
// -------------------------------------------------------
// New particle system
// -------------------------------------------------------
struct NewParticleSystemData {

	uint32 id;
	uint32 maxParticles;
	Rect textureRect;
	uint32 textureID;
};

class NewParticleSystem : public GameObject , public Serializer {

typedef std::vector<ParticleModifier*> Modifiers;

public:
	NewParticleSystem() : GameObject() {
		m_Array.initialize(4096);
	}
	virtual ~NewParticleSystem() {
		clear();
	}

	void init() {}

	void clear() {
		Modifiers::iterator it = m_Modifiers.begin();
		while ( it != m_Modifiers.end()) {
			delete (*it);
			it = m_Modifiers.erase(it);
		}
		m_Emitter.clear();
	}

	void init(const NewParticleSystemData& data) {
		m_Data = data;
	}

	void init(const Rect& r,uint32 textureID) {
		m_Data.textureID = textureID;
		m_Data.textureRect = r;
	}

	void update(float elapsed) {
		//m_Emitter.generate(&m_Array,m_Position,elapsed);
		PR_START("NewParticleSystem::update")
		for ( size_t i = 0;i < m_Modifiers.size(); ++i ) {
			m_Modifiers[i]->update(&m_Array,elapsed);
		}
		PR_END("NewParticleSystem::update")
	}
	void render() {
		PR_START("NewParticleSystem::render")
		if ( m_Array.countAlive > 0 ) {		
			m_Renderer->draw(m_Data.textureID, m_Data.textureRect, m_Array);			
		}
		PR_END("NewParticleSystem::render")
	}
	ParticleEmitter& getEmitter() {
		return m_Emitter;
	}
	NewParticleSystemData& getParticleData() {
		return m_Data;
	}
	void addGenerator(ParticleGenerator* generator) {
		m_Emitter.add(generator);
}
	void start(const Vector2f& startPosition) {
		m_Position = startPosition;
		m_Emitter.generate(&m_Array,m_Position,0.0f);
	}
	void addModifier(ParticleModifier* modifier) {
		m_Modifiers.push_back(modifier);
	}
	void setPosition(const Vector2f& position) {
		m_Position = position;
	}
	void load(BinaryLoader* loader);
private:
	Vector2f m_Position;
	NewParticleSystemData m_Data;
	ParticleSettings m_Settings;
	ParticleArray m_Array;
	Modifiers m_Modifiers;
	ParticleEmitter m_Emitter;
};

class ScriptedParticleSystem : public GameObject {

public:
	ScriptedParticleSystem() : GameObject() , m_Array(4096) {}

	virtual ~ScriptedParticleSystem() {}

	void init() {}
	
	void init(const Rect& r, uint32 textureID,const char* scriptName) {
		m_TextureID = textureID;
		m_TextureRect = r;
		m_Assets->load(scriptName, &m_ParticleScript, CVT_NASM);
		nasm::debug(m_ParticleScript);
	}
	
	void update(float elapsed) {
		PRS("ScriptedParticleSystem::update")
		if (m_Array.size() > 0) {
			nasm::patch_constant(m_ParticleScript, "dt", Vector4f(elapsed));
			PRS("ScriptedParticleSystem::update::TTL")
			Vector4f* timer = m_Array.begin(4);
			for (int i = 0; i < m_Array.size(); ++i) {
				timer->x += elapsed;
				timer->z = timer->x / timer->y;
				++timer;
			}
			PRE("ScriptedParticleSystem::update::TTL")
			nasm::execute(m_ParticleScript, "move", m_Array);
			PRS("ScriptedParticleSystem::update::KILL")
			timer = m_Array.begin(4);
			uint32 cnt = 0;
			while (cnt < m_Array.size()) {
				if (timer->x >= timer->y) {
					m_Array.kill(cnt);
				}
				++cnt;
				++timer;
			}			
			PRE("ScriptedParticleSystem::update::KILL")
			

		}
		PRE("ScriptedParticleSystem::update")
	}
	void render() {
		PRS("ScriptedParticleSystem::render")
		if (m_Array.size() > 0) {
			gfx::draw(m_TextureID, m_Array);
		}
		PRE("ScriptedParticleSystem::render")
	}	
	void start(const Vector2f& startPosition) {
		m_Position = startPosition;		
		nasm::emitt(m_ParticleScript, "emitter", startPosition.x, startPosition.y, m_Array);
	}	
	void setPosition(const Vector2f& position) {
		m_Position = position;
	}	
private:
	nasm::ASMScript m_ParticleScript;
	Vector2f m_Position;
	NParticleArray m_Array;
	int m_TextureID;
	Rect m_TextureRect;
};

}