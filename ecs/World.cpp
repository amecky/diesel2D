#include "World.h"
#include "..\utils\Profiler.h"

namespace ds {

World::World(void) : m_Counter(0) , m_ActorCallback(0) {
	m_CollisionSystem.setActorArray(&m_Actors);
}


World::~World(void) {
	Behaviors::iterator it = m_Behaviors.begin();
	while ( it != m_Behaviors.end() ) {
		delete (*it);
		it = m_Behaviors.erase(it);
	}
	BlueprintMap::iterator bit = m_BlueprintMap.begin();
	while ( bit != m_BlueprintMap.end() ) {
		delete bit->second;
		bit = m_BlueprintMap.erase(bit);
	}
}

void World::attachSprite(ID id,int layer,const ds::Rect& textureRect,float rotation,const Vector2f& scale,const ds::Color& color) {
	Actor& actor = m_Actors.get(id);
	actor.angle = rotation;
	ID spriteID = m_SpriteRenderer->add(id,layer,textureRect,scale,color);
	actor.spriteID = spriteID;
}

void World::attachCollider(ID actorID,float radius) {
	Actor& actor = m_Actors.get(actorID);
	ID id = m_CollisionSystem.add(actorID,radius,actor.type);
	actor.colliderID = id;
}

void World::remove(ID id) {
	if ( m_Actors.contains(id)) {
		Actor& actor = m_Actors.get(id);
		if ( m_ActorCallback != 0 ) {
			m_ActorCallback->onDeactivation(actor);
		}
		if ( actor.spriteID != UINT_MAX ) {
			m_SpriteRenderer->remove(actor.spriteID);
		}
		if ( actor.colliderID != UINT_MAX ) {
			m_CollisionSystem.remove(actor.colliderID);
		}
		m_Actors.remove(id);
	}
	else {
		LOG << "no matching actor found " << id;
	}
}

void World::addBehavior(ID actorID, const char* name) {
	IdString hash = string::murmur_hash(name);
	assert(m_BehaviorMap.find(hash) != m_BehaviorMap.end());
	Actor& entry = m_Actors.get(actorID);
	entry.index[entry.numBehaviors++] = m_BehaviorMap[hash];
}

void  World::updateBehaviors(float elapsed) {
	uint32 cnt = 0;
	for (uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		Actor& actor = m_Actors.objects[i];
		for ( int j = 0; j < actor.numBehaviors; ++j ) {
			Behavior* component = m_Behaviors[actor.index[j]];
			component->update(actor,elapsed);
		}
	}
}

void World::update(float elapsed) {
	PR_START("World::Update")
	// reset collisions
	m_CollisionSystem.reset();
	// tick all actors
	for ( uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		m_Actors.objects[i].timer += elapsed;
	}
	updateBehaviors(elapsed);
	m_SpriteRenderer->update(elapsed);
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
	// handle collisions
	m_CollisionSystem.checkIntersections();
	if ( m_CollisionSystem.getCollisionCounter() > 0 ) {
		int total = m_CollisionSystem.getCollisionCounter();
		ds::ActorCollision col;
		for ( uint32 i = 0; i < total; ++i) {
			const ds::ActorCollision& col = m_CollisionSystem.getCollision(i);
			for ( size_t j = 0; j < m_Callbacks.size(); ++j ) {
				// FIXME: check why there are sometimes invalid IDs (no longer valid)
				if ( m_Actors.contains(col.firstId) && m_Actors.contains(col.secondId)) {
					m_Callbacks[j]->onCollision(col);
				}
			}
		}
	}
	PR_END("World::Update")
}

// -------------------------------------------------------
// Register behavior by name
// -------------------------------------------------------
void World::registerBehavior(const char* name,Behavior* behavior) {
	IdString hash = string::murmur_hash(name);
	if ( m_BehaviorMap.find(hash) == m_BehaviorMap.end()) {
		uint32 idx = find(behavior);
		if ( idx == UINT_MAX ) {
			idx = m_Behaviors.size();
			m_Behaviors.push_back(behavior);
		}
		m_BehaviorMap[hash] = idx;
	}
	else {
		uint32 idx = m_BehaviorMap[hash];
		assert(m_Behaviors[idx] == behavior);
	}
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
	Blueprint* bp = new Blueprint;
	bp->load(name,this);
	IdString hash = string::murmur_hash(name);
	m_BlueprintMap[hash] = bp;
}

// -------------------------------------------------------
// Debug
// -------------------------------------------------------
void World::debug() {
	LOG << ">> Actors";
	for ( uint32 i = 0; i < m_Actors.numObjects; ++i ) {
		LOG << i << " id " << m_Actors.objects[i].id << " pos " << DBG_V2(m_Actors.objects[i].position) << " behaviors " << m_Actors.objects[i].numBehaviors;
	}
	LOG << ">> CollisionSystem";
	m_CollisionSystem.debug();
	LOG << ">> SpriteSystem";
	m_SpriteRenderer->debug();
}

}