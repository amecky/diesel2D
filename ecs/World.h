#pragma once
#include <vector>
#include <map>
#include "Actor.h"
#include "SpriteRenderSystem.h"
#include "CollisionSystem.h"
#include "LifecycleBehaviors.h"
#include "ControllerBehaviors.h"
#include "MoveBehaviors.h"
#include "Blueprint.h"

namespace ds {

class World {

typedef std::vector<ds::CollisionCallback*> Callbacks;
typedef std::map<IdString,uint32> BehaviorMap;
typedef std::map<IdString,Blueprint*> BlueprintMap;

public:

	typedef std::vector<Behavior*> Behaviors;

	World(void);
	~World(void);
	void update(float elapsed);
	ID create(uint32 type,const Vector2f& pos,float angle = 0.0f) {
		ID id = m_Actors.add();
		Actor& actor = m_Actors.get(id);
		actor.clear();
		actor.position = pos;
		actor.angle = angle;
		actor.type = type;
		return id;
	}
	ID createByBlueprint(const char* name) {
		IdString hash = string::murmur_hash(name);
		assert(m_BlueprintMap.find(hash) != m_BlueprintMap.end());
		Blueprint* bp = m_BlueprintMap[hash];
		return bp->initialize(this);
	}
	ID createByBlueprint(const char* name,const Vector2f& pos,float angle = 0.0f) {
		ID id = createByBlueprint(name);
		Actor& actor = m_Actors.get(id);
		actor.position = pos;
		actor.angle = angle;
		return id;
	}
	Actor& getActor(ID id) {
		return m_Actors.get(id);
	}
	void attachSprite(ID actorID,int layer,const ds::Rect& textureRect,float rotation = 0.0f,const Vector2f& scale = Vector2f(1,1),const ds::Color& color = ds::Color::WHITE);
	void attachCollider(ID actorID,float radius);
	void setSpriteRenderer(SpriteRenderSystem* spriteRenderSystem) {
		m_SpriteRenderer = spriteRenderSystem;
		m_SpriteRenderer->setActorArray(&m_Actors);
	}
	void registerCollisionCallback(ds::CollisionCallback* callback) {
		m_Callbacks.push_back(callback);

	}
	void remove(ID actorID);
	void registerActorCallback(ActorCallback* actorCallback) {
		m_ActorCallback = actorCallback;
	}
	void debug();
	const Vector2f& getPosition(ID actorID) {
		return m_Actors.get(actorID).position;
	}
	void addAnimation(ID actorID,const char* name) {
		Actor& actor = m_Actors.get(actorID);
		if ( actor.spriteID != UINT_MAX ) {
			m_SpriteRenderer->addAnimation(actor.spriteID,name);
		}
	}
	void addBehavior(ID actorID, const char* name);
	void updateBehaviors(float elapsed);
	void removeBehaviors(ID actorID);
	void registerAnimation(const char* name,Animation* animation) {
		m_SpriteRenderer->registerAnimation(name,animation);
	}
	template<class A>
	A* getAnimation(const char* name) {
		return m_SpriteRenderer->getAnimation<A>(name);
	}
	void registerBehavior(const char* name,Behavior* behavior);
	template<class B>
	B* getBehavior(const char* name) {
		IdString hash = string::murmur_hash(name);
		if ( m_BehaviorMap.find(hash) != m_BehaviorMap.end()) {
			int idx = m_BehaviorMap[hash];
			return static_cast<B*>(m_Behaviors[idx]);
		}
		return 0;
	}
	void loadBlueprint(const char* name);
	void getActorsByType(std::vector<Actor>& actors,uint32 type);
private:
	uint32 find(Behavior* behavior) {
		for ( size_t i = 0; i < m_Behaviors.size(); ++i ) {
			if ( behavior == m_Behaviors[i] ) {
				return i;
			}
		}
		return UINT_MAX;
	}
	Behaviors m_Behaviors;
	uint32 m_Counter;
	DataArray<Actor,MAX_ITEMS> m_Actors;
	SpriteRenderSystem* m_SpriteRenderer;
	ds::CollisionSystem m_CollisionSystem;
	Callbacks m_Callbacks;
	ActorCallback* m_ActorCallback;
	BehaviorMap m_BehaviorMap;
	BlueprintMap m_BlueprintMap;

};

}