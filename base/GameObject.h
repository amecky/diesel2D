#pragma once
#include "EventStream.h"
#include "..\audio\AudioManager.h"

namespace ds {

//class ParticleManager;
class AssetCompiler;

// -------------------------------------------------------
// GameObject
// -------------------------------------------------------
class GameObject {

public:
	GameObject() {}
	virtual ~GameObject() {}	
	//void setParticleManager(ParticleManager* particleManager) {
		//m_Particles = particleManager;
	//}
	void setAssetCompiler(AssetCompiler* assetCompiler) {
		m_Assets = assetCompiler;
	}
	void setAudioManager(AudioManager* audio) {
		m_Audio = audio;
	}
	virtual void render() {}
	virtual void update(float elapsed) = 0;
	virtual void init() {}
	EventStream& getEvents() {
		return m_Events;
	}
	void resetEvents() {
		m_Events.reset();
	}
	const bool hasEvents() const {
		return m_Events.num() > 0;
	}
	void setActive(bool active) {
		m_Active = active;
	}
	const bool isActive() const {
		return m_Active;
	}
protected:
	EventStream m_Events;
	//ParticleManager* m_Particles;
	AssetCompiler* m_Assets;
	AudioManager* m_Audio;
private:
	GameObject(const GameObject& other) {}
	bool m_Active;
};

}