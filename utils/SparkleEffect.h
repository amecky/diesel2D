#pragma once
#include <Vector.h>
#include "..\renderer\render_types.h"

namespace ds {

	struct SparkleEffectSettings {
		int square_size;
		int max_x;
		int max_y;
		float gap;
		float ttl;
		float startScale;
		float endScale;
		float velocity;
		float velocityVariance;
		float warmupTimer;
	};

	struct SparkleArray {

		v2* positions;
		v2* velocities;
		v2* scales;
		float* rotations;
		float* timers;
		ds::Texture* textures;
		char* buffer;

		int count;
		int countAlive;

		SparkleArray() : count(0), countAlive(0), buffer(0) {}

		~SparkleArray() {
			if (buffer != 0) {
				delete[] buffer;
			}
		}

		void initialize(unsigned int maxParticles) {
			int size = maxParticles * (sizeof(v2) * 3 + sizeof(float) * 2 + sizeof(ds::Texture));
			buffer = new char[size];
			positions = (v2*)(buffer);
			velocities = (v2*)(positions + maxParticles);
			scales = (v2*)(velocities + maxParticles);
			rotations = (float*)(scales + maxParticles);
			timers = (float*)(rotations + maxParticles);
			textures = (ds::Texture*)(timers + maxParticles);
			count = maxParticles;
			countAlive = 0;
		}

		void swapData(uint32 a, uint32 b) {
			if (a != b) {
				positions[a] = positions[b];
				velocities[a] = velocities[b];
				scales[a] = scales[b];
				rotations[a] = rotations[b];
				timers[a] = timers[b];
				textures[a] = textures[b];
			}
		}

		void kill(uint32 id) {
			if (countAlive > 0) {
				swapData(id, countAlive - 1);
				--countAlive;
			}
		}

		void wake(uint32 id) {
			if (countAlive < count)	{
				swapData(id, countAlive);
				++countAlive;
			}
		}

	};

	class SparkleEffect {

	public:
		SparkleEffect(const SparkleEffectSettings& settings);
		virtual ~SparkleEffect();
		void start(int x, int y, const ds::Rect& r, int pieces);
		virtual void update(float dt);
		virtual void render();
		void reset() {
			_sparkles.countAlive = 0;
		}
	protected:
		SparkleArray _sparkles;
		SparkleEffectSettings _settings;
	private:
		int _startX;
		int _startY;
	};


	class IntroEffect : public SparkleEffect {

	public:
		IntroEffect(const SparkleEffectSettings& settings);
		~IntroEffect() {}
		void start(int x, int y, const ds::Rect& r, int stepX, int stepY, float gap, float textureSize = 1024.0f);
		virtual void update(float dt);
		bool isRunning() {
			return _running;
		}
		virtual void render();
	private:
		ds::Texture _texture;
		float _warmUpTimer;
		bool _running;
	};

}