#include "World.h"
#include "..\utils\Profiler.h"
#include "LifecycleBehaviors.h"
#include "Animations.h"

namespace ds {

namespace wo {

	static Actors actors;

	ID create(uint32 layer, uint32 type, const Vector2f& pos, const Vector4f& uv, float angle, const Vector2f& scale, const Color& color) {
		assert(layer < 16);
		ID id = actors.add();
		Actor& actor = actors.get(id);
		actor.clear();
		actor.position = pos;
		actor.layer = layer;
		actor.uv = uv;
		actor.angle = angle;
		actor.scale = scale;
		actor.color = color;
		actor.type = type;
		return id;
	}

	// -------------------------------------------------------
	// Render
	// -------------------------------------------------------
	void render() {
		PR_START("wo::Render")
		for (int i = 0; i < 16; ++i) {
			for (uint32 j = 0; j < actors.numObjects; ++j) {
				Actor& actor = actors.objects[j];
				if (actor.layer == i && actor.active) {
					//m_Renderer->draw(actor.position, 0, actor.textureRect, actor.angle, actor.scale.x, actor.scale.y, actor.color);
				}
			}
		}
		PR_END("wo::Render")
	}

}

World::World(void) : m_ActorCallback(0) {
	m_CollisionSystem.setActorArray(&m_Actors);
	m_Behaviors.add(new FixedLifetimeBehavior);
	m_Behaviors.add(new BulletBehavior);		
	m_Behaviors.add(new TargetMoveBehavior);		
	m_Behaviors.add(new RangeTargetMoveBehavior);		
	m_Behaviors.add(new GravityBehavior);
	m_Behaviors.add(new SimpleMoveBehavior);
	m_Behaviors.add(new BounceMoveBehavior);
	m_Behaviors.add(new ColorFadeAnimation);
	m_Behaviors.add(new SizeAnimation);
	m_Behaviors.add(new GrowSizeAnimation);
	m_Behaviors.add(new StaticRotationAnimation);
	m_Behaviors.add(new VelocityDampingBehavior);
	m_Behaviors.add(new WASDControllerBehavior);
	m_Behaviors.add(new ADControllerBehavior);
	m_Behaviors.add(new BoundingRectBehavior);
	m_Behaviors.add(new SideScrollingBehavior);
}


World::~World(void) {
	BlueprintMap::iterator bit = m_BlueprintMap.begin();
	while ( bit != m_BlueprintMap.end() ) {
		delete bit->second;
		bit = m_BlueprintMap.erase(bit);
	}
	Behaviors::iterator it = m_Behaviors.begin();
	while ( it != m_Behaviors.end() ) {
		delete (*it);
		it = m_Behaviors.erase(it);
	}
}

// -------------------------------------------------------
// create new actor
// -------------------------------------------------------
ID World::create(uint32 layer,uint32 type,const Vector2f& pos,const Rect& textureRect,float angle,const Vector2f& scale,const Color& color) {
	assert(layer < 16);
	ID id = m_Actors.add();
	Actor& actor = m_Actors.get(id);
	actor.clear();
	actor.position = pos;
	actor.layer = layer;
	actor.textureRect = textureRect;
	actor.angle = angle;
	actor.scale = scale;
	actor.color = color;
	actor.type = type;
	return id;
}

// -------------------------------------------------------
// Attach collider to actor
// -------------------------------------------------------
void World::attachCollider(ID actorID,float radius) {
	Actor& actor = m_Actors.get(actorID);
	ID id = m_CollisionSystem.add(actorID,radius,actor.type);
	actor.colliderID = id;
}

// -------------------------------------------------------
// Remove actor 
// -------------------------------------------------------
void World::remove(ID id) {
	Actor& actor = m_Actors.get(id);
	if ( m_ActorCallback ) {
		m_ActorCallback->onDeactivation(actor);
	}
	if ( actor.colliderID != UINT_MAX ) {
		m_CollisionSystem.remove(actor.colliderID);
	}	
	for ( uint32 j = 0; j < actor.numBehaviors; ++j ) {
		Behavior* component = m_Behaviors[actor.index[j]];
		component->remove(actor);
	}
	m_Actors.remove(id);
}

Behavior* World::findBehavior(const char* name) {
	for ( uint32 i = 0; i < m_Behaviors.size(); ++i ) {
		if ( strcmp(name,m_Behaviors[i]->getName()) == 0 ) {
			return m_Behaviors[i];
		}
	}
	return 0;
}

int World::findBehaviorIndex(const char* name) {
	for ( uint32 i = 0; i < m_Behaviors.size(); ++i ) {
		if ( strcmp(name,m_Behaviors[i]->getName()) == 0 ) {
			return i;
		}
	}
	return -1;
}
// -------------------------------------------------------
// add behavior to actor
// -------------------------------------------------------
Behavior* World::addBehavior(ID actorID, const char* name) {
	int bidx = findBehaviorIndex(name);
	assert(bidx != -1);
	Actor& entry = m_Actors.get(actorID);
	entry.index[entry.numBehaviors++] = bidx;
	Behavior* behavior = m_Behaviors[bidx];
	behavior->prepareData(entry);
	behavior->onCreate(entry);
	return behavior;
}

// -------------------------------------------------------
// add behavior to actor
// -------------------------------------------------------
Behavior* World::addBehavior(ID actorID, uint32 index) {
	Actor& entry = m_Actors.get(actorID);
	Behavior* behavior = m_Behaviors[index];
	behavior->prepareData(entry);
	entry.index[entry.numBehaviors++] = index;
	behavior->onCreate(entry);
	return behavior;
}

// -------------------------------------------------------
// Update all actors and their behaviors
// -------------------------------------------------------
void  World::updateBehaviors(float elapsed) {
	uint32 cnt = 0;
	for (uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		Actor& actor = m_Actors.objects[i];
		for ( uint32 j = 0; j < actor.numBehaviors; ++j ) {
			Behavior* component = m_Behaviors[actor.index[j]];
			component->update(actor,elapsed);
		}
	}	
}

// -------------------------------------------------------
// Update world
// -------------------------------------------------------
void World::update(float elapsed) {
	PR_START("World::Update")
	// reset collisions
	m_CollisionSystem.reset();
	// tick all actors
	PRS("World::Update::tick")
	for ( uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		m_Actors.objects[i].timer += elapsed;
	}
	PRE("World::Update::tick")
	PR_START("World::Update::updateBehaviors")
	updateBehaviors(elapsed);
	PR_END("World::Update::updateBehaviors")

	PR_START("World::Update::executeScripts")
	for (uint32 i = 0; i < m_Actors.numObjects; ++i) {
		Actor& actor = m_Actors.objects[i];
		if ( actor.script != -1 ) {
			as::AssemblerScript& script = m_Scripts[actor.script];
			script.context.data[0] = elapsed;
			script.context.data[1] = actor.timer;
			as::reset(script);
			as::registerVar(script, &actor.position);
			as::registerVar(script, &actor.angle);
			as::registerVar(script, &actor.scale);
			as::registerVar(script, &actor.color);
			as::registerVar(script, &actor.velocity);
			as::execute(script);
			if (script.context.event == 666) {
				actor.alive = false;
			}
		}
	}
	PR_END("World::Update::executeScripts")
	PR_START("World::Update::remove")
	// remove dead actors
	ID ids[MAX_ITEMS];
	uint32 cnt = 0;
	for ( uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		Actor& actor = m_Actors.objects[i];
		if ( !actor.alive ) {
			ids[cnt++] = actor.id;
		}
	}	
	for ( uint32 i = 0; i < cnt; ++i ) {
		remove(ids[i]);
	}
	PR_END("World::Update::remove")
	// handle collisions
	PR_START("World::Update::checkIntersections")
	m_CollisionSystem.checkIntersections();
	if ( m_CollisionSystem.getCollisionCounter() > 0 ) {
		uint32 total = m_CollisionSystem.getCollisionCounter();
		ds::ActorCollision col;
		for ( uint32 i = 0; i < total; ++i) {
			const ds::ActorCollision& col = m_CollisionSystem.getCollision(i);
			for ( size_t j = 0; j < m_Callbacks.size(); ++j ) {
				if ( m_Actors.contains(col.firstId) && m_Actors.contains(col.secondId)) {
					m_Callbacks[j]->onCollision(col);
				}
			}
		}
	}
	PR_END("World::Update::checkIntersections")
	PR_END("World::Update")
}

// -------------------------------------------------------
// Render
// -------------------------------------------------------
void World::render() {
	PR_START("World::Render")
	for ( int i = 0; i < 16; ++i ) {
		for ( uint32 j = 0; j < m_Actors.numObjects; ++j ) {
			Actor& actor = m_Actors.objects[j];
			if ( actor.layer == i && actor.active ) {
				//m_Renderer->draw(actor.position,0,actor.textureRect,actor.angle,actor.scale.x,actor.scale.y,actor.color);
			}
		}
	}
	PR_END("World::Render")
}

// -------------------------------------------------------
// get actors by type
// -------------------------------------------------------
void World::getActorsByType(std::vector<Actor>& actors,uint32 type) {
	for ( uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		Actor& actor = m_Actors.objects[i];
		if ( actor.type == type ) {
			actors.push_back(actor);
		}
	}
}
// -------------------------------------------------------
// load blueprint
// -------------------------------------------------------
void World::loadBlueprint(const char* name) {
	Blueprint* bp = new Blueprint(this);
	m_Assets->load(name,bp,CVT_BLUEPRINT);
	IdString hash = string::murmur_hash(name);
	m_BlueprintMap[hash] = bp;
}

// -------------------------------------------------------
// Debug
// -------------------------------------------------------
void World::debug() {
	/*
	LOG << ">> Behaviors";
	for ( size_t i = 0; i < m_Behaviors.size(); ++i ) {
		LOG << m_Behaviors[i]->getName();
	}
	*/
	LOG << ">> Actors";
	for ( uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		Actor& actor = m_Actors.objects[i];
		LOG << i << " id: " << actor.id << " type: " << actor.type << " pos: " << DBG_V2(actor.position) << " behaviors: " << actor.numBehaviors;
	}
	LOG << ">> CollisionSystem";
	m_CollisionSystem.debug();
}

}