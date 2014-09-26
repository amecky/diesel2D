#include "StateMachine.h"
#include "..\utils\StringUtils.h"

namespace ds {

StateMachine::StateMachine() : m_Current(0) {
}


StateMachine::~StateMachine() {
	// delete all states
	States::iterator it = m_States.begin();
	while ( it != m_States.end() ) {
		delete (*it).second;
		it = m_States.erase(it);
	}
}

void StateMachine::registerState(const char* name,ds::GameState* state) {
	IdString hash = ds::string::murmur_hash(name);
	m_States[hash] = state;
}

void StateMachine::activate(const char* name) {
	if ( m_Current != 0 ) {
		m_Current->onDeactivation();
	}
	IdString hash = ds::string::murmur_hash(name);
	m_Current = m_States[hash];
	m_Current->onActivation();
}

void StateMachine::update(float dt) {
	if ( m_Current != 0 ) {
		m_Current->update(dt);
	}
}

void StateMachine::render() {
	PRS("StateMachine::draw")
	if ( m_Current != 0 ) {
		m_Current->render();
	}
	PRE("StateMachine::draw")
}

void StateMachine::onGUIButton(ds::DialogID dlgID,int button) {
	if ( m_Current != 0 ) {
		m_Current->onGUIButton(dlgID,button);
	}
}

void StateMachine::OnButtonDown(int button,int x,int y) {
	if ( m_Current != 0 ) {
		m_Current->onButtonDown(button,x,y);
	}
}

}