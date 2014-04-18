#pragma once
#include "EventStream.h"
#include "..\renderer\Renderer.h"
#include "..\sprites\SpriteCollisionManager.h"

namespace ds {

class TFunctor {

public:
	virtual void updateState(Sprite* sprite,float elapsed) {}
};	

template <class TClass> 
class TSpecificFunctor : public TFunctor
   {
   private:
      void (TClass::*fpt)(Sprite*,float);   // pointer to member function
      TClass* pt2Object;                  // pointer to object

   public:

	      // constructor - takes pointer to an object and pointer to a member and stores
      // them in two private variables
      TSpecificFunctor(TClass* _pt2Object, void(TClass::*_fpt)(Sprite*,float))
         { pt2Object = _pt2Object;  fpt=_fpt; };

      // override function "Call"
      virtual void updateState(Sprite* sprite,float elapsed)
        { (*pt2Object.*fpt)(sprite,elapsed);};             // execute member function
   };

class ParticleManager;
class AssetCompiler;

// -------------------------------------------------------
// GameObject
// -------------------------------------------------------
class GameObject {

typedef std::vector<TFunctor*> Callbacks;

public:
	GameObject() , m_ActiveCallback(-1) {}
	virtual ~GameObject() {}	
	void setRenderer(Renderer* renderer) {
		m_Renderer = renderer;
	}
	void setCollisionManager(SpriteCollisionManager* collisionManager) {
		m_CollisionManager = collisionManager;
	}
	void setParticleManager(ParticleManager* particleManager) {
		m_Particles = particleManager;
	}
	void setAssetCompiler(AssetCompiler* assetCompiler) {
		m_Assets = assetCompiler;
	}
	virtual void update(float elapsed) = 0;
	virtual void render() = 0;
	virtual void init() {}
	const bool isActive() const {
		return m_Active;
	}
	void setActive(bool active) {
		m_Active = active;
	}
	EventStream& getEvents() {
		return m_Events;
	}
	void resetEvents() {
		m_Events.reset();
	}
	const bool hasEvents() const {
		return m_Events.num() > 0;
	}
	void registerFN(TFunctor* fn) {
		m_Callbacks.push_back(fn);
	}
	//void call(int i){
		//CALL_MEMBER_FN(*this, m_Callback) (0, 3.14f);
	//}
protected:
	Callbacks m_Callbacks;
	int m_ActiveCallback;
	//GameObjectFn m_Callback;
	EventStream m_Events;
	Renderer* m_Renderer;
	SpriteCollisionManager* m_CollisionManager;
	ParticleManager* m_Particles;
	AssetCompiler* m_Assets;
private:
	GameObject(const GameObject& other) {}
	bool m_Active;
};

}