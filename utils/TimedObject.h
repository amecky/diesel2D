#pragma once

namespace ds {

	struct ObjectTimer {

		float timer;
		float normalizedTimer;

		ObjectTimer() : timer(0.0f), normalizedTimer(1.0f) {}

		bool tickTimer(float dt, float threshold, bool reset = true) {
			timer += dt;
			if (threshold != 0.0f) {
				normalizedTimer = timer / threshold;
			}
			if (timer >= threshold) {
				normalizedTimer = 1.0f;
				if (reset) {
					timer -= threshold;
				}
				return true;
			}
			return false;
		}

		void resetTimer() {
			timer = 0.0f;
		}
	};

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