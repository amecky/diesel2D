#pragma once
#include "..\world\World.h"
#include "..\particles\ParticleManager.h"
#include <map>

namespace ds {

	class Trails {

		enum TrailEmitType {
			TET_DISTANCE,
			TET_TIME,
			TET_FRAMES
		};

		struct TrailPiece {

			Vector2f prevPosition;
			int particleSystem;
			union {
				float distance;
				int frames;
				float time;
			};
			union {
				float timer;
				int frameCounter;
			};
			float angle;
			TrailEmitType emitType;

		};

		typedef std::map<SID, TrailPiece> Pieces;

	public:
		Trails(World* world, ParticleManager* particles);
		~Trails(void);
		void add(SID sid, float distance, int particleSystem);
		void add(SID sid, float angle, float distance, int particleSystem);
		void addFrameBased(SID sid, float angle, int frames, int particleSystem);
		void addTimeBased(SID sid, float angle, float ttl, int particleSystem);
		void tick(float dt);
		void remove(SID sid);
	private:
		World* _world;
		ParticleManager* _particles;
		Pieces _pieces;
	};

}