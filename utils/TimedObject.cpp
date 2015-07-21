#include "TimedObject.h"

namespace ds {
	TimedObject::TimedObject() : _ticking(false), _timer(0.0f) {
	}


	TimedObject::~TimedObject()	{
	}

	void TimedObject::tickTimer(float dt) {
		if (_ticking) {
			_timer += dt;
		}
	}

	bool TimedObject::tickTimer(float dt, float threshold, bool reset) {
		bool ret = false;
		if (_ticking) {
			_timer += dt;
			if (_timer >= threshold) {
				ret = true;
				if (reset) {
					_timer = 0.0f;
				}
			}
		}
		return ret;

	}

	void TimedObject::startTimer() {
		_ticking = true;
		_timer = 0.0f;
	}

	void TimedObject::resumeTimer() {
		_ticking = true;
	}

	void TimedObject::stopTimer() {
		_ticking = false;
	}
}
