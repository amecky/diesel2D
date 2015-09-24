#pragma once
#include "GameState.h"
#include "..\utils\Log.h"

namespace ds {

class GameStateMachine	{

typedef std::vector<GameState*> GameStates;

public:
	GameStateMachine() {
		_activeState = 0;
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
		t->setStateMachine(this);
		t->init();
		_gameStates.push_back(t);
	}
	void add(GameState* gameState) {
		gameState->setStateMachine(this);
		gameState->init();
		_gameStates.push_back(gameState);
	}
	void activate(const char* name) {
		int idx = find(name);
		if (idx != -1) {
			if (_activeState != 0) {
				_activeState->deactivate();
			}
			_activeState = _gameStates[idx];
			_activeState->activate();
		}
		else {
			LOGE << "Cannot find state: '" << name << "'";
		}
	}
	bool contains(const char* name) const {
		return find(name) != -1;
	}
	void update(float dt) {
		if (_activeState != 0) {
			_activeState->update(dt);
		}
	}
	void render() {
		if (_activeState != 0) {
			_activeState->render();
		}
	}
	void onButtonDown(int button, int x, int y) {
		if (_activeState != 0) {
			_activeState->onButtonDown(button, x, y);
		}
	}
	void onButtonUp(int button, int x, int y) {
		if (_activeState != 0) {
			_activeState->onButtonUp(button, x, y);
		}
	}
	void onChar(int ascii) {
		if (_activeState != 0) {
			_activeState->onChar(ascii);
		}
	}
	void onGUIButton(ds::DialogID dlgID, int button) {
		if (_activeState != 0) {
			_activeState->onGUIButton(dlgID, button);
		}
	}
private:
	int find(const char* name) const {
		IdString hash = string::murmur_hash(name);
		for (size_t i = 0; i < _gameStates.size(); ++i) {		
			if (_gameStates[i]->getHash() == hash) {
				return i;
			}
		}
		return -1;
	}
	GameState* _activeState;
	GameStates _gameStates;
};

}
