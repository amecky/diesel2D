#include "GameStateMachine.h"
#include "..\utils\Log.h"
#include "..\utils\Profiler.h"

namespace ds {

	GameStateMachine::GameStateMachine() {
		_activeState = 0;
		_currentIndex = -1;
		_dialogPos = v2(50, 700);
		_dialogState = 1;
		_offset = 0;
	}

	GameStateMachine::~GameStateMachine() {
		_model.clear();
		GameStates::iterator it = _gameStates.begin();
		while (it != _gameStates.end()) {
			delete (*it);
			it = _gameStates.erase(it);
		}
	}

	void GameStateMachine::add(GameState* gameState) {
		_gameStates.push_back(gameState);
		_model.add(gameState->getName(), gameState);
	}

	void GameStateMachine::initializeStates() {
		for (size_t i = 0; i < _gameStates.size(); ++i) {
			_gameStates[i]->init();
		}
	}
	void GameStateMachine::activate(const char* name) {
		int idx = find(name);
		assert(idx != -1);
		if (idx != -1) {
			switchState(idx);
		}
	}

	bool GameStateMachine::contains(const char* name) const {
		return find(name) != -1;
	}

	void GameStateMachine::update(float dt) {
		if (_activeState != 0) {
			int transition = _activeState->update(dt);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::render() {
		if (_activeState != 0) {
			_activeState->render();
		}
	}

	void GameStateMachine::onButtonDown(int button, int x, int y) {
		if (_activeState != 0) {
			int transition = _activeState->onButtonDown(button, x, y);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::onKeyDown(WPARAM virtualKey) {
		if (_activeState != 0) {
			int transition = _activeState->onKeyDown(virtualKey);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::onKeyUp(WPARAM virtualKey) {
		if (_activeState != 0) {
			int transition = _activeState->onKeyUp(virtualKey);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::onButtonUp(int button, int x, int y) {
		if (_activeState != 0) {
			int transition = _activeState->onButtonUp(button, x, y);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::onChar(int ascii) {
		if (_activeState != 0) {
			int transition = _activeState->onChar(ascii);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::onGUIButton(ds::DialogID dlgID, int button) {
		if (_activeState != 0) {
			int transition = _activeState->onGUIButton(dlgID, button);
			handleStateTransition(transition);
		}
	}

	void GameStateMachine::connect(const char* firstStateName, int outcome, const char* secondStateName) {
		assert(outcome != 0);
		FSMConnection connection;
		connection.firstStateIndex = find(firstStateName);
		assert(connection.firstStateIndex != -1);
		connection.secondStateIndex = find(secondStateName);
		assert(connection.secondStateIndex != -1);
		connection.outcome = outcome;
		_connections.push_back(connection);
	}

	void GameStateMachine::handleStateTransition(int outcome) {
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

	int GameStateMachine::find(const char* name) const {
		IdString hash = string::murmur_hash(name);
		for (size_t i = 0; i < _gameStates.size(); ++i) {
			if (_gameStates[i]->getHash() == hash) {
				return i;
			}
		}
		return -1;
	}

	void GameStateMachine::switchState(int newIndex) {
		if (_activeState != 0) {
			_activeState->deactivate();
		}
		_activeState = _gameStates[newIndex];
		_activeState->activate();
		_currentIndex = newIndex;
	}

	void GameStateMachine::showDialog() {
		PR_START("GameStateMachine::showDialog")
		gui::start(101,&_dialogPos);
		if (gui::begin("GameStates", &_dialogState)) {
			gui::ComboBox(100, &_model, &_offset);
			if (gui::Button(101, "Activate")) {
				if (_model.hasSelection()) {
					GameState* state = _model.getSelectedValue();
					activate(state->getName());
				}
			}
		}
		gui::end();
		PR_END("GameStateMachine::showDialog")
	}
}
