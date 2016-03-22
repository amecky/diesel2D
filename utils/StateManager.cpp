#include "StateManager.h"
#include "Log.h"

namespace ds {

	// -------------------------------------------------------
	// State Manager
	// -------------------------------------------------------	
	StateManager::~StateManager() {
		// remove all states
		std::map<int, State*>::iterator it = _states.begin();
		while (it != _states.end()) {
			delete it->second;
			++it;
		}

	}

	// -------------------------------------------------------
	// add transition
	// -------------------------------------------------------
	void StateManager::addTransition(int from, int outcome, int to, float ttl) {
		// FIXME: assert that from and to are valid
		XASSERT(from != to, "You cannot add a transition pointing to itself");
		XASSERT(_states.find(from) != _states.end(), "The from %d paramter is an unknown state",from);
		XASSERT(_states.find(to) != _states.end(), "The to %d paramter is an unknown state",to);
		StateTransition t;
		t.from = from;
		t.to = to;
		t.outcome = outcome;
		t.ttl = ttl;
		_transitions.push_back(t);
	}

	// -------------------------------------------------------
	// find transition
	// -------------------------------------------------------
	int StateManager::findTransition(int from, int outcome) {
		for (int i = 0; i < _transitions.size(); ++i) {
			const StateTransition& t = _transitions[i];
			if (t.from == from && t.outcome == outcome) {
				return i;
			}
		}
		return -1;
	}

	// -------------------------------------------------------
	// activate
	// -------------------------------------------------------
	void StateManager::activate(int mode) {
		_next = mode;
	}

	// -------------------------------------------------------
	// switch state
	// -------------------------------------------------------
	bool StateManager::switchState() { 
		if (_next != -1 && _next != _current) {
			int outcome = 0;
			if (_current != -1) {
				outcome = _states[_current]->deactivate();
				LOG << "deactivating: " << _states[_current]->getName() << " outcome: " << outcome;
				if (outcome != 0) {
					int idx = findTransition(_current, outcome);
					if (idx != -1) {
						_current = _transitions[idx].to;
					}
					else {
						_current = -1;
					}
				}
				else {
					_current = _next;
				}
			}
			else {
				_current = _next;
			}
			if (_current != -1) {
				_timer = 0.0f;
				outcome = _states[_current]->activate();
				LOG << "activating: " << _states[_current]->getName() << " outcome: " << outcome;
				if (outcome != 0) {
					int idx = findTransition(_current, outcome);
					if (idx != -1) {
						activate(_transitions[idx].to);
					}
				}
				int idx = findTransition(_current, 0);
				if (idx != -1 && _transitions[idx].ttl > 0.0f) {
					LOG << "setting transient state";
					_transient = true;
					_ttl = _transitions[idx].ttl;
				}
				else {
					_transient = false;
				}
			}
			else {
				LOG << "No active state!";
			}
			_next = -1;
		}
		return false;
	}

	// -------------------------------------------------------
	// tick
	// -------------------------------------------------------
	void StateManager::tick(float dt) {
		_events.reset();
		if (!switchState()) {
			if (_current != -1) {			
				// update current state
				int outcome = _states[_current]->update(dt);
				if (_transient) {
					_timer += dt;
					if (_timer >= _ttl) {
						LOG << "timer expired";
						int idx = findTransition(_current, outcome);
						if (idx != -1) {
							StateTransition t = _transitions[idx];
							activate(t.to);
						}
						else {
							LOG << "no next state";
							_current = -1;
						}
					}
				}
				if (outcome != 0) {
					int idx = findTransition(_current, outcome);
					if (idx != -1) {
						activate(_transitions[idx].to);
					}
					else {
						_current = -1;
					}
				}
			}
		}
	}

	// -------------------------------------------------------
	// stop
	// -------------------------------------------------------
	void StateManager::stop() {
		if (_current != -1) {
			_states[_current]->deactivate();
		}
		_current = -1;
	}

}