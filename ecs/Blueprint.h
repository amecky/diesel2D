#pragma once
#include <Vector.h>
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include "..\data\DataTranslator.h"
#include "Actor.h"
#include "..\io\Serializer.h"


namespace ds {

// -------------------------------------------------------
// collider definition
// -------------------------------------------------------
struct ColliderDefinition {

	float radius;
	bool used;

	ColliderDefinition() : radius(0.0f) , used(false) {}
};

// -------------------------------------------------------
// actor definition
// -------------------------------------------------------
struct ActorDefinition {

	Vector2f position;
	float angle;
	int type;
	int layer;
	Rect textureRect;
	Vector2f scale;
	Color color;
};

// -------------------------------------------------------
// Behavior definition
// -------------------------------------------------------
struct BehaviorDefinition {

	int index;
	ID definitionID;

};

class World;
// -------------------------------------------------------
// Blueprint
// -------------------------------------------------------
class Blueprint : public Serializer {

typedef std::vector<uint32> Behaviors;
typedef std::vector<BehaviorDefinition> BehaviorDefinitions;

public:
	Blueprint(World* world);
	~Blueprint(void);
	virtual void load( BinaryLoader* loader );
	ID initialize(World* world);
private:
	DataTranslator<ActorDefinition> m_ActorTranslator;
	ActorDefinition m_ActorDefinition;
	ColliderDefinition m_ColliderDefinition;
	DataTranslator<ColliderDefinition> m_ColliderTranslator;
	Behaviors m_Behaviors;
	World* m_World;
	BehaviorDefinitions m_BehaviorDefinitions;
};

}