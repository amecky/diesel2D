#pragma once
#include "..\..\math\Vector.h"
#include "..\lib\DataArray.h"
#include "..\dxstdafx.h"
#include <vector>
#include "..\utils\Log.h"
#include "..\utils\Color.h"
#include "..\math\math_types.h"
#include "..\data\DataTranslator.h"
#include "..\data\DynamicSettings.h"

const uint32 MAX_ITEMS = 512;

namespace ds {

struct Actor {

	Vector2f position;
	Vector2f velocity;
	float angle;
	ID id;
	ID spriteID;
	ID colliderID;
	uint32 type;
	float timer;
	bool alive;

	uint32 index[16];
	uint32 numBehaviors;

	Actor() : id(UINT_MAX) {
		clear();
	}
	
	void clear() {
		position = Vector2f(0,0);
		velocity = Vector2f(0,0);
		angle = 0.0f;
		spriteID = UINT_MAX;
		colliderID = UINT_MAX;
		type = UINT_MAX;
		timer = 0.0f; 
		alive = true;
		numBehaviors = 0;
	}
};

// -------------------------------------------------------
// Behavior 
// -------------------------------------------------------
class Behavior {

public:
	Behavior() {}
	virtual ~Behavior() {}
	virtual void update(Actor& actor,float elapsed) = 0;
	DynamicSettings& getSettings() {
		return m_Settings;
	}
protected:
	DynamicSettings m_Settings;

};

// -------------------------------------------------------
// Actor callback
// -------------------------------------------------------
class ActorCallback {

public:
	void onUpdate(Actor* actor) {}
	virtual void onDeactivation(const Actor& actor) = 0;
};

// -------------------------------------------------------
// Sprite
// -------------------------------------------------------
struct sprite_t {

	uint32 layer;
	ID id;
	ID actorID;
	Color color;
	Rect textureRect;
	Vector2f scale;

	int animations[16];
	uint32 numAnimations;

	sprite_t() : layer(0) , id(0) , actorID(0) , color(Color::WHITE) , scale(1,1) , numAnimations(0) {}

};

// -------------------------------------------------------
// Animation
// -------------------------------------------------------
class Animation {

public:
	virtual void update(Actor& actor,sprite_t& sprite,float elapsed) = 0;
	DynamicSettings& getSettings() {
		return m_Settings;
	}
protected:
	DynamicSettings m_Settings;
};

}