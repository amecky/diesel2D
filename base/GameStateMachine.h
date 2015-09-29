#pragma once
#include "GameState.h"
#include "..\utils\Log.h"

namespace ds {

class GameStateMachine	{

struct FSMConnection {
	int firstStateIndex;
	int secondStateIndex;
	int outcome;
};

typedef std::vector<GameState*> GameStates;
typedef std::vector<FSMConnection> Connections;

public:
	GameStateMachine() {
		_activeState = 0;
		_currentIndex = -1;
	}
	~GameStateMachine() {
		GameStates::iterator it = _gameStates.begin();
		while (it != _gameStates.end()) {
			delete (*it);
			it = _gameStates.erase(it);
		}
	}
	template<class T>
	void add(const char* name) {
		T* t = new T(name);
		t->init();
		_gameStates.push_back(t);
	}
	void add(GameState* gameState) {
		_gameStates.push_back(gameState);
	}
	void initializeStates() {
		for (size_t i = 0; i < _gameStates.size(); ++i) {
			_gameStates[i]->init();
		}
	}
	void activate(const char* name) {
		int idx = find(name);
		assert(idx != -1);
		if (idx != -1) {
			switchState(idx);			
		}
	}
	bool contains(const char* name) const {
		return find(name) != -1;
	}
	void update(float dt) {
		if (_activeState != 0) {
			int transition = _activeState->update(dt);
			handleStateTransition(transition);
		}
	}
	void render() {
		if (_activeState != 0) {
			_activeState->render();
		}
	}
	void onButtonDown(int button, int x, int y) {
		if (_activeState != 0) {
			int transition = _activeState->onButtonDown(button, x, y);
			handleStateTransition(transition);
		}
	}
	void onButtonUp(int button, int x, int y) {
		if (_activeState != 0) {
			int transition = _activeState->onButtonUp(button, x, y);
			handleStateTransition(transition);
		}
	}
	void onChar(int ascii) {
		if (_activeState != 0) {
			int transition = _activeState->onChar(ascii);
			handleStateTransition(transition);
		}
	}
	void onGUIButton(ds::DialogID dlgID, int button) {
		if (_activeState != 0) {
			int transition = _activeState->onGUIButton(dlgID, button);
			handleStateTransition(transition);
		}
	}
	void connect(const char* firstStateName,int outcome, const char* secondStateName) {
		assert(outcome != 0);
		FSMConnection connection;
		connection.firstStateIndex = find(firstStateName);
		assert(connection.firstStateIndex != -1);
		connection.secondStateIndex = find(secondStateName);
		assert(connection.secondStateIndex != -1);		
		connection.outcome = outcome;
		_connections.push_back(connection);
	}
private:
	void handleStateTransition(int outcome) {
		if (outcome != 0) {
			for (size_t i = 0; i < _connections.size(); ++i) {
				const FSMConnection& con = _connections[i];
				if (con.firstStateIndex == _currentIndex && con.outcome == outcome) {
					switchState(con.secondStateIndex);
					break;
				}
			}
		}
	}
	int find(const char* name) const {
		IdString hash = string::murmur_hash(name);
		for (size_t i = 0; i < _gameStates.size(); ++i) {		
			if (_gameStates[i]->getHash() == hash) {
				return i;
			}
		}
		return -1;
	}
	void switchState(int newIndex) {
		if (_activeState != 0) {
			_activeState->deactivate();
		}
		_activeState = _gameStates[newIndex];
		_activeState->activate();
		_currentIndex = newIndex;
	}
	int _currentIndex;
	GameState* _activeState;
	GameStates _gameStates;
	Connections _connections;
};

}
