#pragma once
#include "GameState.h"
#include "..\utils\Log.h"
#include "..\ui\IMGUI.h"
#include "..\lib\collection_types.h"
#include "..\editor\AssetEditor.h"

namespace ds {

class GameStateMachine	: public AssetEditor {

struct FSMConnection {
	int firstStateIndex;
	int secondStateIndex;
	int outcome;
};

typedef Array<GameState*> GameStates;
typedef Array<FSMConnection> Connections;

public:
	GameStateMachine();
	~GameStateMachine();
	template<class T>
	void add(const char* name) {
		T* t = new T(name);
		t->init();
		_gameStates.push_back(t);
		_model.add(name, t);
	}
	void add(GameState* gameState);
	void initializeStates();
	void activate(const char* name);
	bool contains(const char* name) const;
	void update(float dt);
	void render();
	void onButtonDown(int button, int x, int y);
	void onKeyDown(WPARAM virtualKey);
	void onKeyUp(WPARAM virtualKey);
	void onButtonUp(int button, int x, int y);
	void onChar(int ascii);
	void onGUIButton(ds::DialogID dlgID, int button);
	void connect(const char* firstStateName, int outcome, const char* secondStateName);
	void showDialog();
	void init() {}
	const char* getShortcut() const {
		return "GSM";
	}
private:
	void handleStateTransition(int outcome);
	int find(const char* name) const;
	void switchState(int newIndex);
	int _currentIndex;
	GameState* _activeState;
	GameStates _gameStates;
	Connections _connections;
	gui::ComponentModel<GameState*> _model;
	v2 _dialogPos;
	int _dialogState;
	int _offset;
};

}
