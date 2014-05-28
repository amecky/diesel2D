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
};

// -------------------------------------------------------
// Target movement component
// -------------------------------------------------------
class TargetMoveBehavior : public Behavior {

public:
	TargetMoveBehavior() : Behavior() , m_Velocity(100.0f) {
		m_Settings.add("velocity",&m_Velocity);
	}
	TargetMoveBehavior(float velocity) : Behavior() , m_Velocity(velocity) {
		m_Settings.add("velocity",&m_Velocity);
	}
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
private:
	bool m_Rotate;
	float m_Velocity;
	Vector2f m_TargetPos;
};

// -------------------------------------------------------
// Bounce movement behavior
// -------------------------------------------------------
class BounceMoveBehavior : public Behavior {

public:
	BounceMoveBehavior() : Behavior() , m_BoundingRect(0,0,1024,768) , m_Damping(1.0f) {
		m_Settings.add("bounding_rect",&m_BoundingRect);
		m_Settings.add("damping",&m_Damping);
	}
	BounceMoveBehavior(const Rect& r) : Behavior() , m_BoundingRect(r) , m_Damping(1.0f) {
		m_Settings.add("bounding_rect",&m_BoundingRect);
		m_Settings.add("damping",&m_Damping);
	}
	virtual ~BounceMoveBehavior() {}
	void update(Actor& actor,float elapsed) {		
		if ( actor.position.x < m_BoundingRect.left ) {
			actor.position.x = m_BoundingRect.left + 2.0f;
			actor.velocity.x *= -m_Damping;
		}
		else if ( actor.position.x > m_BoundingRect.right ) {
			actor.position.x = m_BoundingRect.right - 2.0f;
			actor.velocity.x *= -m_Damping;
		}
		if ( actor.position.y < m_BoundingRect.top ) {
			actor.position.y = m_BoundingRect.top + 2.0f;
			actor.velocity.y *= -m_Damping;
		}
		else if ( actor.position.y > m_BoundingRect.bottom ) {
			actor.position.y = m_BoundingRect.bottom - 2.0f;
			actor.velocity.y *= -m_Damping;
		}
	}
private:
	Rect m_BoundingRect;
	float m_Damping;
};

// -------------------------------------------------------
// Bullet movement component
// -------------------------------------------------------
class BulletBehavior : public Behavior {

public:
	BulletBehavior() : Behavior() , m_BoundingRect(0,0,1024,768) {
		m_Settings.add("bounding_rect",&m_BoundingRect);
	}
	BulletBehavior(const Rect& boundingRect) : Behavior() , m_BoundingRect(boundingRect) {
		m_Settings.add("bounding_rect",&m_BoundingRect);
	}
	void update(Actor& actor,float elapsed) {
		actor.position += actor.velocity * elapsed;
		if ( actor.position.x < m_BoundingRect.left || actor.position.x > m_BoundingRect.right ) {
			actor.alive = false;
		}
		if ( actor.position.y < m_BoundingRect.top || actor.position.y > m_BoundingRect.bottom ) {
			actor.alive = false;
		}
	}
	void setBoundingRect(const Rect& boundingRect) {
		m_BoundingRect = boundingRect;
	}
private:
	Rect m_BoundingRect;
};

// -------------------------------------------------------
// gravity movement behavior
// -------------------------------------------------------
class GravityBehavior : public Behavior {

public:
	GravityBehavior() : Behavior() , m_Velocity(20.0f) {
		m_Settings.add("acceleration",&m_Velocity);
	}
	void update(Actor& actor,float elapsed) {
		actor.velocity.y -= m_Velocity * elapsed;
		actor.position += actor.velocity * elapsed;
	}
private:
	float m_Velocity;
};


}