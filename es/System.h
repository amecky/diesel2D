#pragma once
#include "EntityManager.h"

// -----------------------------------------
// Base data
// -----------------------------------------
struct BaseData {

	Entity entity;
};

// -----------------------------------------
// System data
// -----------------------------------------
template<class data>
struct SystemData : BaseData {

};
// -------------------------------------------------------
// BaseSystem
// -------------------------------------------------------
class BaseSystem {

public:
	BaseSystem(EntityManager* entityManager) {
		m_EntityManager = entityManager;
	}
	virtual ~BaseSystem() {}
	virtual void update(float dt) = 0;
	virtual void addEntity(const Entity& entity) = 0;
	virtual void removeData(const Entity& entity) = 0;
	virtual const ESMode getMode() = 0;
	virtual void remove(const Entity& entity) {}
	
protected:
	EntityManager* m_EntityManager;
	
};

// -------------------------------------------------------
// System
// -------------------------------------------------------
template<typename Data>
class System : public BaseSystem {

public:
	System(EntityManager* entityManager) : BaseSystem(entityManager) {}
	virtual ~System() {}
	void removeData(const Entity& entity);	
	template<class C>
	C* getComponent(const Entity& entity) {
		return m_EntityManager->getComponent<C>(entity);
	}
protected:	
	std::vector<Data> m_Data;
};

// -------------------------------------------------------
// Remove data
// -------------------------------------------------------
template<typename Data>
void System<Data>::removeData(const Entity& entity) {	
	std::vector<Data>::iterator it = m_Data.begin();
	while ( it != m_Data.end() ) {
		if ( it->entity == entity ) {
			it = m_Data.erase(it);
		}
		else {
			++it;
		}
	}	
}




