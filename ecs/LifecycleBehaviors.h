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
	BoundingRectBehavior() : m_BoundingRect(0,0,1024,768) {}
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
	
	const char* getName() const {
		return "BoundingRectBehavior";
	}
private:
	ds::Rect m_BoundingRect;
};

// -------------------------------------------------------
// Fixed lifetime
// -------------------------------------------------------
struct FixedLifetimeData {

	ID id;
	ID actorID;
	float ttl;

	FixedLifetimeData() : ttl(1.0f) {
		//add("ttl",&ttl);
	}
};

class FixedLifetimeBehavior : public AbstractBehavior<FixedLifetimeData> {

public:
	FixedLifetimeBehavior() : AbstractBehavior<FixedLifetimeData>() {
		m_Translator.add("ttl",&FixedLifetimeData::ttl);
	}
	virtual ~FixedLifetimeBehavior() {}
	
	void init(const Actor& actor,ID definitionID) {
		FixedLifetimeData& def = m_Definitions.get(definitionID);
		FixedLifetimeData& data = getData(actor);
		data.ttl = def.ttl;
	}

	void update(Actor& actor,float elapsed) {
		FixedLifetimeData& data = getData(actor);
		if ( actor.timer >= data.ttl ) {
			actor.alive = false;
		}
	}

	void setTimeToLive(const Actor& actor,float ttl) {
		FixedLifetimeData& data = getData(actor);
		data.ttl = ttl;
	}

	const char* getName() const {
		return "FixedLifetimeBehavior";
	}

};
}