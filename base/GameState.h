#pragma once
#include "BaseApp.h"
#include "..\dialogs\GUIDialog.h"

namespace ds {

class GameState {

public:
	GameState(const char* name) : _name(name) {
		_hash = string::murmur_hash(name);
	}
	void setStateMachine(GameStateMachine* machine) {
		stateMachine = machine;
	}
	virtual ~GameState(void) {}
	virtual void init() {}
	virtual void update(float dt) {}
	virtual void render() {}
	// callbacks
	virtual void activate() {}
	virtual void deactivate() {}
	virtual void onButtonDown(int button,int x,int y) {}
	virtual void onButtonUp(int button, int x, int y) {}
	virtual void onChar(int ascii) {}
	virtual void onGUIButton(ds::DialogID dlgID, int button) {}
	IdString getHash() const {
		return _hash;
	}
protected:
	GameStateMachine* stateMachine;
private:
	const char* _name;
	IdString _hash;
};

}