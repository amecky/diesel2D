#pragma once
#include "EventStream.h"
#include "..\renderer\Renderer.h"

namespace ds {
// -------------------------------------------------------
// GameObject
// -------------------------------------------------------
class GameObject {

public:
	GameObject() {}
	virtual ~GameObject() {}	
	void setRenderer(Renderer* renderer) {
		m_Renderer = renderer;
	}
	virtual void update(float elapsed) = 0;
	virtual void render() = 0;
	virtual void init() {}
	const bool isActive() const {
		return m_Active;
	}
	void setActive(bool active) {
		m_Active = active;
	}
	EventStream& getEvents() {
		return m_Events;
	}
	void resetEvents() {
		m_Events.reset();
	}
	const bool hasEvents() const {
		return m_Events.num() > 0;
	}
protected:
	EventStream m_Events;
	Renderer* m_Renderer;
private:
	GameObject(const GameObject& other) {}
	bool m_Active;
};

}