#pragma once
#include "..\lib\container\dVector.h"
#include <map>
#include "Actor.h"
#include "CollisionSystem.h"
#include "LifecycleBehaviors.h"
#include "ControllerBehaviors.h"
#include "MoveBehaviors.h"
#include "Blueprint.h"
#include "..\renderer\Renderer.h"
#include "..\compiler\AssetCompiler.h"
#include "..\script\Assembler.h"

namespace ds {

typedef DataArray<Actor, MAX_ITEMS> Actors;

namespace wo {

	ID create(uint32 layer, uint32 type, const Vector2f& pos, const Vector4f& uv, float angle = 0.0f, const Vector2f& scale = Vector2f(1, 1), const Color& color = Color::WHITE);

}

class World {

typedef std::vector<ds::CollisionCallback*> Callbacks;
typedef std::map<IdString,Blueprint*> BlueprintMap;

typedef dVector<Behavior*> Behaviors;
typedef std::vector<as::AssemblerScript> Scripts;

public:
	World(void);
	~World(void);

	void update(float elapsed);

	ID create(uint32 layer,uint32 type,const Vector2f& pos,const Rect& textureRect,float angle = 0.0f,const Vector2f& scale = Vector2f(1,1),const Color& color = Color::WHITE);
	
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
	void attachCollider(ID actorID,float radius);
	
	void registerCollisionCallback(ds::CollisionCallback* callback) {
		m_Callbacks.push_back(callback);

	}
	void remove(ID actorID);
	void registerActorCallback(ActorCallback* actorCallback) {
		m_ActorCallback = actorCallback;
	}
	void debug();
	Behavior* findBehavior(const char* name);
	int findBehaviorIndex(const char* name);
	Behavior* addBehavior(ID actorID, const char* name);
	Behavior* addBehavior(ID actorID, uint32 index);
	void updateBehaviors(float elapsed);
	void removeBehaviors(ID actorID);
	void registerBehavior(Behavior* behavior) {
		m_Behaviors.add(behavior);
	}
	template<class B>
	B* getBehavior(const char* name) {
		return (B*)findBehavior(name);
		//return m_Registry.getBehavior<B>(name);
	}
	void loadBlueprint(const char* name);
	void getActorsByType(std::vector<Actor>& actors,uint32 type);
	void render();
	void setRenderer(Renderer* renderer) {
		m_Renderer = renderer;
	}
	void setAssetCompiler(AssetCompiler* assetCompiler) {
		m_Assets = assetCompiler;
	}
	int loadScript(const char* name) {
		int ret = m_Scripts.size();
		as::AssemblerScript script;
		m_Scripts.push_back(script);
		as::AssemblerScript& current = m_Scripts[m_Scripts.size() - 1];
		m_Assets->load(name, &current, CVT_ASM);				
		return ret;
	}
	void attachScript(ID id, int scriptID) {
		Actor& a = getActor(id);
		a.script = scriptID;
	}
private:
	Actors m_Actors;
	ds::Renderer* m_Renderer;
	//ActorRegistry m_Registry;
	Scripts m_Scripts;
	ds::CollisionSystem m_CollisionSystem;
	Callbacks m_Callbacks;
	BlueprintMap m_BlueprintMap;
	ActorCallback* m_ActorCallback;
	AssetCompiler* m_Assets;
	Behaviors m_Behaviors;

};

}