#pragma once
#include "..\lib\DataArray.h"
#include "Actor.h"
#include "..\math\math_types.h"
#include <vector>
#include "..\utils\Log.h"

namespace ds {
// -------------------------------------------------------
// Bounding rect life cycle behavior
// -------------------------------------------------------
class BoundingRectBehavior : public Behavior {

public:
	BoundingRectBehavior() : m_BoundingRect(0,0,1024,768) {
		m_Settings.add("bounding_rect",&m_BoundingRect);
	}
	BoundingRectBehavior(const Rect& r) : m_BoundingRect(r) {
		m_Settings.add("bounding_rect",&m_BoundingRect);
	}
	virtual ~BoundingRectBehavior() {}
	void update(Actor& actor,float elapsed) {
		Vector2f pos = actor.position;
		if ( pos.x < m_BoundingRect.left || pos.x > m_BoundingRect.right ) {
			actor.alive = false;
		}
		if ( pos.y < m_BoundingRect.top || pos.y > m_BoundingRect.bottom ) {
			actor.alive = false;
		}
	}
	void setBoundingRect(const ds::Rect& boundingRect) {
		m_BoundingRect = boundingRect;
	}
private:
	ds::Rect m_BoundingRect;
};

// -------------------------------------------------------
// Fixed lifetime
// -------------------------------------------------------
class FixedLifetimeBehavior : public Behavior {

public:
	FixedLifetimeBehavior() : m_TTL(1.0f) {
		m_Settings.add("ttl",&m_TTL);
	}
	FixedLifetimeBehavior(float ttl) : m_TTL(ttl) {
		m_Settings.add("ttl",&m_TTL);
	}
	virtual ~FixedLifetimeBehavior() {}
	void update(Actor& actor,float elapsed) {
		if ( actor.timer >= m_TTL ) {
			actor.alive = false;
		}
	}
	void setTimeToLive(float ttl) {
		m_TTL = ttl;
	}
private:
	float m_TTL;
};
}