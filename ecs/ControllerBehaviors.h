#pragma once
#include "..\..\math\Vector.h"
#include "..\dxstdafx.h"
#include "..\math\GameMath.h"
#include "Actor.h"
#include "..\lib\DataArray.h"
#include <vector>
#include "..\utils\Log.h"

namespace ds {

class WASDControllerBehavior : public Behavior {

public:
	WASDControllerBehavior() : Behavior() , m_Velocity(0.0f) , m_Rotate(false) , m_FollowPosition(0,0) {}
	WASDControllerBehavior(float velocity,bool rotate = false) : Behavior() , m_Velocity(velocity) , m_Rotate(rotate) , m_FollowPosition(0,0) {}
	virtual ~WASDControllerBehavior() {}
	void update(Actor& actor,float elapsed) {
		Vector2f pos = actor.position;
		bool moved = false;
		if( GetAsyncKeyState('W') & 0x8000 ) {
			pos.y += m_Velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('S') & 0x8000 ) {
			pos.y -= m_Velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('A') & 0x8000 ) {
			pos.x -= m_Velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('D') & 0x8000 ) {
			pos.x += m_Velocity * elapsed;
			moved = true;
		}		
		float angle = 0.0f;
		if ( m_Rotate ) {
			actor.angle = ds::math::getTargetAngle(pos,m_FollowPosition);
		}
		if ( moved ) {
			actor.position = pos;
			angle = ds::math::getTargetAngle(Vector2f(1,0),pos);
		}
		
	}
	void setVelocity(float velocity) {
		m_Velocity = velocity;
	}
	void setFollowPosition(const Vector2f& followPosition) {
		m_FollowPosition = followPosition;
		m_Rotate = true;
	}
private:
	float m_Velocity;
	Vector2f m_FollowPosition;
	bool m_Rotate;
};

// -------------------------------------------------------
// AD left right controller
// -------------------------------------------------------
class ADControllerBehavior : public Behavior {

public:
	ADControllerBehavior() : Behavior() , m_Velocity(0.0f) , m_Rotate(false) , m_FollowPosition(0,0) {
		m_Settings.add("velocity",&m_Velocity);
	}
	ADControllerBehavior(float velocity) : Behavior() , m_Velocity(velocity) , m_Rotate(false) , m_FollowPosition(0,0) {
		m_Settings.add("velocity",&m_Velocity);
	}
	virtual ~ADControllerBehavior() {}
	void update(Actor& actor,float elapsed) {
		Vector2f pos = actor.position;
		bool moved = false;
		if( GetAsyncKeyState('A') & 0x8000 ) {
			pos.x -= m_Velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('D') & 0x8000 ) {
			pos.x += m_Velocity * elapsed;
			moved = true;
		}		
		//float angle = 0.0f;
		//if ( m_Rotate ) {
			//actor.angle = ds::math::getTargetAngle(pos,m_FollowPosition);
		//}
		if ( moved ) {
			actor.position = pos;
			//angle = ds::math::getTargetAngle(Vector2f(1,0),pos);
		}

	}
private:
	float m_Velocity;
	Vector2f m_FollowPosition;
	bool m_Rotate;
};

}