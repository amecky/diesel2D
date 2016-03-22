#pragma once
#include "..\lib\collection_types.h"
#include <map>
#include "..\base\EventStream.h"

namespace ds {

	// -------------------------------------------------------
	// State type
	// -------------------------------------------------------
	struct StateType {
		const char* name;
		int type;
	};

	// -------------------------------------------------------
	// State context
	// -------------------------------------------------------
	struct StateContext {

	};

	// -------------------------------------------------------
	// Base State
	// -------------------------------------------------------
	class State {

	public:
		State() {}
		virtual ~State() {}
		void prepare(StateContext* context, EventStream* events) {
			_ctx = context;
			_events = events;
		}
		virtual void init() {

		}
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
		void sendEvent(uint32_t type) {
			_events->add(type);
		}
	protected:
		StateContext* _ctx;
	private:
		EventStream* _events;
	};

	// -------------------------------------------------------
	// State transition
	// -------------------------------------------------------
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
		StateManager(StateContext* ctx) : _current(-1) , _next(-1) {
			_context = ctx;
		}
		~StateManager();
		template<class T>
		State* add() {
			T* t = new T();
			t->prepare(_context, &_events);
			t->init();
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
		bool hasEvents() const {
			return _events.num() > 0;
		}
		const EventStream& getEventStream() const {
			return _events;
		}
	private:
		bool switchState();
		int findTransition(int from, int outcome);
		StateContext* _context;
		std::map<int, State*> _states;
		Array<StateTransition> _transitions;
		EventStream _events;
		int _current;
		int _next;
		float _timer;
		float _ttl;
		bool _transient;
	};
}