#pragma once
#include <vector>
#include <stdio.h>
#include "..\utils\Log.h"
#include "..\math\math_types.h"
#include "..\memory\MallocAllocator.h"

typedef unsigned int Entity;
typedef unsigned int uint32;

// -------------------------------------------------------
// 
// -------------------------------------------------------
struct BaseComponent {
	
	Entity entity;
	uint32 groupId;
};

// -------------------------------------------------------
// 
// -------------------------------------------------------
template<typename Derived>
struct Component : public BaseComponent {
	
};

class BaseSystem;

enum ESMode {
	ES_UPDATE,
	ES_RENDER
};
// -------------------------------------------------------
// Entity manager
// -------------------------------------------------------
class EntityManager {

typedef std::vector<BaseComponent*> Components;
typedef std::vector<BaseSystem*> Systems;
typedef std::vector<Entity> Entities;

typedef std::vector<Entity> DeleteEntities;

public:
	EntityManager(ds::Allocator& allocator) : m_Index(0) , m_Allocator(&allocator) {		
	}
	~EntityManager(void);
	template<class S>
	S* createSystem();
	Entity createEntity();
	template<class C>
	C* assign(const Entity& entity);
	void add(const Entity& entity);
	void getComponents(const Entity& entity,std::vector<BaseComponent*>& list);
	const std::vector<Entity>& getEntities() const {
		return m_Entities;
	}
	template<class C>
	C* getComponent(const Entity& entity);
	void update(float dt);
	void render();
	void remove(const Entity& entity);
private:
	void cleanup();
	Entities m_Entities;
	DeleteEntities m_DeleteEntities;
	Components m_Components;
	Systems m_Systems;
	uint32 m_Index;
	ds::Allocator* m_Allocator;
};

// -------------------------------------------------------
// Get component for entity
// -------------------------------------------------------
template<class C>
C* EntityManager::getComponent(const Entity& entity) {
	C* tmp = new C;
	for ( size_t i = 0; i < m_Components.size(); ++i ) {
		BaseComponent* bc = m_Components[i];
		if ( bc->entity == entity && bc->groupId == tmp->groupId) {
			return (C*)bc;
		}
	}
	delete tmp;
	return 0;
}
// -------------------------------------------------------
// Create system
// -------------------------------------------------------
template<class S> 
S* EntityManager::createSystem() {
	S* system = m_Allocator->make_new<S>(this);
	//S* system = new S(this);
	m_Systems.push_back(system);
	return system;
}

// -------------------------------------------------------
// Assign
// -------------------------------------------------------
template<class C>
C* EntityManager::assign(const Entity& entity) {
	C* component = m_Allocator->make_new<C>();//new C;
	component->entity = entity;
	m_Components.push_back(component);
	return component;
}



