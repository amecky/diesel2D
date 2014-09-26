#pragma once
#include <map>
#include "GameState.h"
#include "..\dialogs\GUIDialog.h"

namespace ds {

class StateMachine {

typedef std::map<IdString,GameState*> States;

public:
	StateMachine(void);
	~StateMachine(void);
	void registerState(const char* name,GameState* state);
	void activate(const char* name);
	void deactivate(const char* name);
	void update(float dt);
	void render();
	const GameState* getCurrentState() const {
		return m_Current;
	}

	void onGUIButton(ds::DialogID dlgID,int button);
	void OnButtonDown(int button,int x,int y);
	//virtual void OnButtonUp(int button,int x,int y) {}
	//virtual void OnKeyDown( WPARAM wParam ) {}
	//virtual void OnKeyUp( WPARAM wParam ) {}
	//virtual void OnChar(char ascii,unsigned int keyState) {}
private:
	GameState* m_Current;
	States m_States;

};

}

