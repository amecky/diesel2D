#pragma once
#include "..\lib\collection_types.h"
#include <map>

namespace ds {

	struct StateType {
		const char* name;
		int type;
	};

	struct StateContext {

	};

	// -------------------------------------------------------
	// Base State
	// -------------------------------------------------------
	class State {

	public:
		State(StateContext* context) : _ctx(context) {}
		virtual ~State() {}
		virtual int activate() {
			return 0;
		}
		virtual int deactivate() {
			return 0;
		}
		virtual int update(float dt) {
			return 0;
		}
		virtual int getMode() const = 0;
		virtual const char* getName() const = 0;
	protected:
		StateContext* _ctx;
	};

	struct StateTransition {
		int from;
		int to;
		int outcome;
		float ttl;
	};
	// -------------------------------------------------------
	// State manager
	// -------------------------------------------------------
	class StateManager {

	public:
		StateManager(StateContext* ctx) : _current(-1) {
			_context = ctx;
		}
		~StateManager();
		template<class T>
		State* add() {
			T* t = new T(_context);
			_states[t->getMode()] = t;
			return t;
		}
		void addTransition(int from, int outcome, int to, float ttl);
		void activate(int mode);
		void tick(float dt);
		void stop();
		int getCurrentMode() const {
			return _current;
		}
	private:
		int findTransition(int from, int outcome);
		StateContext* _context;
		std::map<int, State*> _states;
		Array<StateTransition> _transitions;
		int _current;
		float _timer;
		float _ttl;
		bool _transient;
	};
}