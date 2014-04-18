#include "EntityManager.h"
#include "System.h"


EntityManager::~EntityManager() {
	LOGC("EntityManager") << "Destructing entity manager";
	Components::iterator cit = m_Components.begin();
	while ( cit != m_Components.end()) {
		//delete (*cit);
		m_Allocator->make_delete((*cit));
		cit = m_Components.erase(cit);
	}
	
	Systems::iterator it = m_Systems.begin();
	while ( it != m_Systems.end() ) {
		//delete (*it);
		m_Allocator->make_delete((*it));
		it = m_Systems.erase(it);
	}
}
// -------------------------------------------------------
// Create entity
// -------------------------------------------------------
uint32 EntityManager::createEntity() {
	uint32 ret = m_Index;
	LOG << "new entity " << ret;
	m_Entities.push_back(ret);
	++m_Index;
	return ret;
}
// -------------------------------------------------------
// Add
// -------------------------------------------------------
void EntityManager::add(const Entity& entity) {
	for ( size_t i = 0; i < m_Systems.size(); ++i ) {
		m_Systems[i]->addEntity(entity);
	}
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void EntityManager::update(float dt) {
	for ( size_t i = 0; i < m_Systems.size(); ++i ) {
		if ( m_Systems[i]->getMode() == ES_UPDATE ) {
			m_Systems[i]->update(dt);
		}
	}
	cleanup();
}

// -------------------------------------------------------
// Render
// -------------------------------------------------------
void EntityManager::render() {
	for ( size_t i = 0; i < m_Systems.size(); ++i ) {
		if ( m_Systems[i]->getMode() == ES_RENDER ) {
			m_Systems[i]->update(0.0f);
		}
	}
}
// -------------------------------------------------------
// Remove
// -------------------------------------------------------
void EntityManager::remove(const Entity& entity) {
	LOG << "marking to remove entity " << entity;
	m_DeleteEntities.push_back(entity);
}

// -------------------------------------------------------
// Clean up entities marked for deletion
// -------------------------------------------------------
void EntityManager::cleanup() {
	DeleteEntities::iterator dit = m_DeleteEntities.begin();
	while ( dit != m_DeleteEntities.end()) {
		Entity entity = *dit;
		LOG << "remove entity " << entity;
		for ( size_t i = 0; i < m_Systems.size(); ++i ) {
			m_Systems[i]->remove(entity);
			m_Systems[i]->removeData(entity);
		}
		Components::iterator it = m_Components.begin();
		while ( it != m_Components.end() ) {
			if ( (*it)->entity == entity ) {
				m_Allocator->make_delete((*it));
				it = m_Components.erase(it);
			}
			else {
				++it;
			}
		}
		dit = m_DeleteEntities.erase(dit);
	}
}

// -------------------------------------------------------
// Get components
// -------------------------------------------------------
void EntityManager::getComponents(const Entity& entity,std::vector<BaseComponent*>& list) {
	for ( size_t i = 0; i < m_Components.size(); ++i ) {
		if ( m_Components[i]->entity == entity ) {
			list.push_back(m_Components[i]);
		}
	}
}

