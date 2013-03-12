#include "GameStateManager.h"
#include "..\utils\Log.h"

namespace ds {

GameStateManager::GameStateManager() : m_PendingActivation(false) {
    //ctor
}

GameStateManager::~GameStateManager() {
    //dtor
	GSMap::iterator iter;
	for ( iter = m_GameStates.begin(); iter != m_GameStates.end(); ++iter ) {
		delete iter->second;		
	}
	m_GameStates.clear();
}

void GameStateManager::addGameState(GameState *gameState) {
	gameState->setActive(false);
    m_GameStates[gameState->getName()] = gameState;
}

GameState* GameStateManager::getActiveState() {
    if ( !m_ActiveName.empty() ) {
        return m_GameStates[m_ActiveName];        
    }
	return NULL;
}

GameState* GameStateManager::get(const std::string& name) {
	return m_GameStates[name];        
}

void GameStateManager::activate(const std::string& name,bool lazyInit) {
    if ( !m_ActiveName.empty() ) {
        GameState *current = m_GameStates[m_ActiveName];
        current->onDeactivation();
		current->setActive(false);
    }
	if ( m_GameStates.find(name) != m_GameStates.end() ) {
		GameState *next = m_GameStates[name];
		next->onActivation();
		next->setActive(true);
		m_ActiveName = name;
	}
}

const bool GameStateManager::isActive(const std::string& name) const {
	return name == m_ActiveName;
}

void GameStateManager::initializeStates(ResManager& resourceManager) {
	GSMap::iterator iter;
	for ( iter = m_GameStates.begin(); iter != m_GameStates.end(); ++iter ) {
		iter->second->init();	
	}
	for ( iter = m_GameStates.begin(); iter != m_GameStates.end(); ++iter ) {
		iter->second->loadContent(resourceManager);	
	}
}

void GameStateManager::debug() {
	GSMap::iterator it = m_GameStates.begin();
	LOGC(logINFO,"GSM") << "active state: " << m_ActiveName;
	while ( it != m_GameStates.end() ) {
		LOGC(logINFO,"GSM") << it->second->getName();
		++it;
	}	
}

}
