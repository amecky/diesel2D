#pragma once
#include "..\lib\DataArray.h"
#include "Actor.h"
#include "..\math\math_types.h"
#include <vector>
#include "..\utils\Log.h"

namespace ds {
// -------------------------------------------------------
// Simple movement component
// -------------------------------------------------------
class SimpleMoveBehavior : public Behavior {

public:
	SimpleMoveBehavior() : Behavior() {}
	virtual ~SimpleMoveBehavior() {}
	void update(Actor& actor,float elapsed) {
		actor.position += actor.velocity * elapsed;
	}
	const uint32 getID() const {
		return 6;
	}
	const char* getName() const {
		return "SimpleMoveBehavior";
	}
};

// -------------------------------------------------------
// side scrolling movement component
// -------------------------------------------------------
class SideScrollingBehavior : public Behavior {

public:
	SideScrollingBehavior() : Behavior() {}
	virtual ~SideScrollingBehavior() {}

	void update(Actor& actor,float elapsed) {
		actor.position += actor.velocity * elapsed;
		if ( actor.position.x < -actor.textureRect.width() ) {
			actor.position.x = 1024 + actor.textureRect.width();
		}
	}

	const char* getName() const {
		return "SideScrollingBehavior";
	}
};

// -------------------------------------------------------
// Target movement component
// -------------------------------------------------------
class TargetMoveBehavior : public Behavior {

public:
	TargetMoveBehavior() : Behavior() , m_Velocity(100.0f) {}
	virtual ~TargetMoveBehavior() {}
	
	void update(Actor& actor,float elapsed) {
		Vector2f diff = m_TargetPos - actor.position;
		Vector2f reaction = normalize(diff);
		reaction *= m_Velocity;
		actor.position +=  reaction * elapsed;
	}
	
	void setTargetPosition(const Vector2f& targetPosition) {
		m_TargetPos = targetPosition;
	}
	
	const char* getName() const {
		return "TargetMoveBehavior";
	}
private:
	bool m_Rotate;
	float m_Velocity;
	Vector2f m_TargetPos;
};

// -------------------------------------------------------
// Bounce movement behavior
// -------------------------------------------------------
struct BounceMoveBehaviorData {

	ID id;
	ID actorID;
	Rect boundingRect;
	float damping;

	BounceMoveBehaviorData() : boundingRect(0,0,1024,768) , damping(1.0f) {}

};

class BounceMoveBehavior : public AbstractBehavior<BounceMoveBehaviorData> {

public:
	BounceMoveBehavior() : AbstractBehavior<BounceMoveBehaviorData>() {
		m_Translator.add("bounding_rect",&BounceMoveBehaviorData::boundingRect);
		m_Translator.add("damping",&BounceMoveBehaviorData::damping);
	}

	virtual ~BounceMoveBehavior() {}

	void init(const Actor& actor,ID definitionID) {
		BounceMoveBehaviorData& def = m_Definitions.get(definitionID);
		BounceMoveBehaviorData& data = getData(actor);
		data.boundingRect = def.boundingRect;
		data.damping = def.damping;
	}
	
	void update(Actor& actor,float elapsed) {		
		BounceMoveBehaviorData& data = getData(actor);
		if ( actor.position.x < data.boundingRect.left ) {
			actor.position.x = data.boundingRect.left + 2.0f;
			actor.velocity.x *= -data.damping;
		}
		else if ( actor.position.x > data.boundingRect.right ) {
			actor.position.x = data.boundingRect.right - 2.0f;
			actor.velocity.x *= -data.damping;
		}
		if ( actor.position.y < data.boundingRect.top ) {
			actor.position.y = data.boundingRect.top + 2.0f;
			actor.velocity.y *= -data.damping;
		}
		else if ( actor.position.y > data.boundingRect.bottom ) {
			actor.position.y = data.boundingRect.bottom - 2.0f;
			actor.velocity.y *= -data.damping;
		}
	}
	
	const char* getName() const {
		return "BounceMoveBehavior";
	}

};
// -------------------------------------------------------
// Bullet movement component
// -------------------------------------------------------
struct BulletBehaviorData {

	ID id;
	ID actorID;
	Rect boundingRect;

	BulletBehaviorData() : boundingRect(0,0,124,768) {
	}
};

class BulletBehavior : public AbstractBehavior<BulletBehaviorData> {

public:
	BulletBehavior() : AbstractBehavior<BulletBehaviorData>() {
		m_Translator.add("bounding_rect",&BulletBehaviorData::boundingRect);
	}

	void update(Actor& actor,float elapsed) {
		BulletBehaviorData& data = getData(actor);
		actor.position += actor.velocity * elapsed;
		if ( actor.position.x < data.boundingRect.left || actor.position.x > data.boundingRect.right ) {
			actor.alive = false;
		}
		if ( actor.position.y < data.boundingRect.top || actor.position.y > data.boundingRect.bottom ) {
			actor.alive = false;
		}
	}

	void init(const Actor& actor,ID definitionID) {
		BulletBehaviorData& def = m_Definitions.get(definitionID);
		BulletBehaviorData& data = getData(actor);
		data.boundingRect = def.boundingRect;
	}

	const char* getName() const {
		return "BulletBehavior";
	}

};

// -------------------------------------------------------
// gravity movement behavior
// -------------------------------------------------------
class GravityBehavior : public Behavior {

public:
	GravityBehavior() : Behavior() , m_Velocity(20.0f) {}

	void update(Actor& actor,float elapsed) {
		actor.velocity.y -= m_Velocity * elapsed;
		actor.position += actor.velocity * elapsed;
	}
	
	const char* getName() const {
		return "GravityBehavior";
	}
private:
	float m_Velocity;
};

// -------------------------------------------------------
// VelocityDampingBehavior
// -------------------------------------------------------
class VelocityDampingBehavior : public Behavior {

public:
	VelocityDampingBehavior() : Behavior() , m_Velocity(0.9f) {}

	void update(Actor& actor,float elapsed) {
		if ( sqr_length(actor.velocity) > 1.0f ) {
			Vector2f v = actor.velocity * m_Velocity * elapsed;
			actor.velocity -= v;			
		}
		else {
			actor.velocity = Vector2f(0,0);
		}
	}

	const char* getName() const {
		return "VelocityDampingBehavior";
	}
private:
	float m_Velocity;
};

// -------------------------------------------------------
// Target movement component
// -------------------------------------------------------
class RangeTargetMoveBehavior : public Behavior {

public:
	RangeTargetMoveBehavior() : Behavior() , m_Velocity(100.0f) , m_Radius(100.0f) {}
	virtual ~RangeTargetMoveBehavior() {}
	
	void update(Actor& actor,float elapsed) {
		Vector2f diff = m_TargetPos - actor.position;
		if ( sqr_length(diff) > m_Radius * m_Radius ) {
			Vector2f reaction = normalize(diff);
			reaction *= m_Velocity;
			actor.position +=  reaction * elapsed;
		}
		else {
			actor.position += actor.velocity * elapsed;
		}
	}
	void setTargetPosition(const Vector2f& targetPosition) {
		m_TargetPos = targetPosition;
	}
	
	const char* getName() const {
		return "RangeTargetMoveBehavior";
	}
private:
	float m_Radius;
	bool m_Rotate;
	float m_Velocity;
	Vector2f m_TargetPos;
};


}