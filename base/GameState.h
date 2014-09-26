#pragma once
#include "BaseApp.h"
#include "..\dialogs\GUIDialog.h"

namespace ds {

class GameState {

public:
	GameState(BaseApp* app) : m_Application(app) {}
	virtual ~GameState(void) {}

	virtual void update(float dt) {}
	virtual void render() {}

	// callbacks
	virtual void onActivation() {}
	virtual void onDeactivation() {}
	virtual void onGUIButton(ds::DialogID dlgID,int button) {}
	virtual void onButtonDown(int button,int x,int y) {}
protected:
	BaseApp* m_Application;
};

}