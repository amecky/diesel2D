#pragma once
#include <map>
#include <string>
#include "..\content\ResManager.h"

namespace ds {

class GameState {

public:
	GameState(const std::string& name) : m_Name(name) , active(false) {}
	virtual ~GameState() {}
    std::string getName() const { 
		return m_Name;
	}
	virtual bool init() {
		return true;
	}
	virtual bool loadContent(ResManager& resourceManager) {
		return true;
	}
	virtual bool update(float elapsedTime) {
		return true;
	}
	virtual bool onActivation() {
		return true;
	}
	virtual bool onDeactivation() {
		return true;
	}
	bool isActive() const { 
		return active;
	}
	void setActive(bool act) { 
		active = act;
	}
private:
    std::string m_Name;
	bool active;
};

class GameStateManager {

typedef std::map<std::string,GameState*> GSMap;

public:
    GameStateManager();
    virtual ~GameStateManager();
    void addGameState(GameState *gameState);
    void activate(const std::string& name,bool lazyInit = false);
	GameState* getActiveState();
	const bool isActive(const std::string& name) const;
	void initializeStates(ResManager& resourceManager);
	GameState* get(const std::string& name);

	void debug();
private:
	bool m_PendingActivation;
    std::string m_ActiveName;
    GSMap m_GameStates;
};

};
