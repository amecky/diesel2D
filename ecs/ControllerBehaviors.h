#pragma once
#include "..\..\math\Vector.h"
#include "..\dxstdafx.h"
#include "..\math\GameMath.h"
#include "Actor.h"
#include "..\lib\DataArray.h"
#include <vector>
#include "..\utils\Log.h"

namespace ds {

struct WASDControllerData {

	ID id;
	ID actorID;
	float velocity;
	bool rotate;

	WASDControllerData() : velocity(100.0f) , rotate(false) {
		//add("velocity",&velocity);
		//add("rotate",&rotate);
	}
};

class WASDControllerBehavior : public AbstractBehavior<WASDControllerData> {

public:
	WASDControllerBehavior() : AbstractBehavior<WASDControllerData>() {}
	virtual ~WASDControllerBehavior() {}

	void update(Actor& actor,float elapsed) {
		WASDControllerData& data = getData(actor);
		Vector2f pos = actor.position;
		bool moved = false;
		if( GetAsyncKeyState('W') & 0x8000 ) {
			pos.y += data.velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('S') & 0x8000 ) {
			pos.y -= data.velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('A') & 0x8000 ) {
			pos.x -= data.velocity * elapsed;
			moved = true;
		}
		if( GetAsyncKeyState('D') & 0x8000 ) {
			pos.x += data.velocity * elapsed;
			moved = true;
		}		
		float angle = 0.0f;
		if ( data.rotate ) {
			actor.angle = ds::math::getTargetAngle(pos,m_FollowPosition);
		}
		if ( moved ) {
			actor.position = pos;
			angle = ds::math::getTargetAngle(Vector2f(1,0),pos);
		}
		
	}
	
	void setVelocity(const Actor& actor,float velocity,bool rotate = false) {
		WASDControllerData& data = getData(actor);
		data.velocity = velocity;
		data.rotate = rotate;
	}

	void setFollowPosition(const Vector2f& followPosition) {
		m_FollowPosition = followPosition;
	}
	
	const char* getName() const {
		return "WASDControllerBehavior";
	}
private:
	Vector2f m_FollowPosition;
};

// -------------------------------------------------------
// AD left right controller
// -------------------------------------------------------
class ADControllerBehavior : public Behavior {

public:
	ADControllerBehavior() : Behavior() {}
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
	
	const char* getName() const {
		return "ADControllerBehavior";
	}
private:
	float m_Velocity;
	Vector2f m_FollowPosition;
	bool m_Rotate;
};

}