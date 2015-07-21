#pragma once

namespace ds {

	class TimedObject {

	public:
		TimedObject();
		~TimedObject();
		void tickTimer(float dt);
		bool tickTimer(float dt, float threshold,bool reset = true);
		void startTimer();
		void stopTimer();
		void resumeTimer();
		const bool isTimerActive() const {
			return _ticking;
		}
		float getElapsed() const {
			return _timer;
		}
		void resetTimer() {
			_timer = 0.0f;
			_ticking = true;
		}
	private:
		float _timer;
		bool _ticking;
	};

}