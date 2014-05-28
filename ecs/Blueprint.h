#pragma once
#include <Vector.h>
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include "..\data\DataTranslator.h"
#include "Actor.h"


namespace ds {

struct ColliderDefinition {

	float radius;
};

struct SpriteDefinition {

	int layer;
	Rect textureRect;
	float angle;
	Vector2f scale;
	Color color;

};

struct ActorDefinition {

	Vector2f position;
	float angle;
	int type;

};

class World;

class Blueprint {

typedef std::vector<std::string> BehaviorNames;
typedef std::vector<std::string> AnimationNames;

public:
	Blueprint(void);
	~Blueprint(void);
	void load(const char* name,World* world);
	ID initialize(World* world);
private:
	Animation* createAnimation(const std::string& name);
	Behavior* createBehavior(const std::string& name);
	DataTranslator<ActorDefinition> m_ActorTranslator;
	ActorDefinition m_ActorDefinition;
	SpriteDefinition* m_SpriteDefinition;
	DataTranslator<SpriteDefinition> m_SpriteTranslator;
	ColliderDefinition* m_ColliderDefinition;
	DataTranslator<ColliderDefinition> m_ColliderTranslator;
	BehaviorNames m_BehaviorNames;
	AnimationNames m_AnimationNames;
};

}