#include "Trails.h"

namespace ds {

	Trails::Trails(World* world, ParticleManager* particles) : _world(world) , _particles(particles) {
	}


	Trails::~Trails() {
	}

	// --------------------------------------------------------------------------
	// add new trail
	// --------------------------------------------------------------------------
	void Trails::add(SID sid, float distance, int particleSystem) {
		TrailPiece tp;
		tp.particleSystem = particleSystem;
		tp.prevPosition = _world->getPosition(sid);
		tp.distance = distance;
		tp.emitType = TET_DISTANCE;
		tp.frameCounter = 0;
		_pieces[sid] = tp;
	}

	// --------------------------------------------------------------------------
	// add new trail with specific rotation
	// --------------------------------------------------------------------------
	void Trails::add(SID sid, float angle, float distance, int particleSystem) {
		TrailPiece tp;
		tp.particleSystem = particleSystem;
		tp.prevPosition = _world->getPosition(sid);
		tp.distance = distance;
		tp.angle = angle;
		tp.emitType = TET_DISTANCE;
		tp.frameCounter = 0;
		_pieces[sid] = tp;
	}

	// --------------------------------------------------------------------------
	// add frame based
	// --------------------------------------------------------------------------
	void Trails::addFrameBased(SID sid, float angle, int frames, int particleSystem) {
		TrailPiece tp;
		tp.particleSystem = particleSystem;
		tp.prevPosition = _world->getPosition(sid);
		tp.frames = frames;
		tp.angle = angle;
		tp.emitType = TET_FRAMES;
		tp.frameCounter = 0;
		_pieces[sid] = tp;
	}

	// --------------------------------------------------------------------------
	// add time based
	// --------------------------------------------------------------------------
	void Trails::addTimeBased(SID sid, float angle, float ttl, int particleSystem) {
		TrailPiece tp;
		tp.particleSystem = particleSystem;
		tp.prevPosition = _world->getPosition(sid);
		tp.time = ttl;
		tp.angle = angle;
		tp.emitType = TET_TIME;
		tp.timer = 0.0f;
		_pieces[sid] = tp;
	}

	// --------------------------------------------------------------------------
	// removes trail
	// --------------------------------------------------------------------------
	void Trails::remove(SID sid) {
		if (_pieces.find(sid) != _pieces.end()) {
			_pieces.erase(sid);
		}
	}

	// --------------------------------------------------------------------------
	// tick
	// --------------------------------------------------------------------------
	void Trails::tick(float dt) {
		PR_START("Trail:tick")
		Pieces::iterator it = _pieces.begin();
		while (it != _pieces.end()) {
			if (_world->contains(it->first)) {
				Vector2f p = _world->getPosition(it->first);
				if (it->second.emitType == TET_DISTANCE) {
					if (distance(p, it->second.prevPosition) > it->second.distance) {
						ParticleGeneratorData data(it->second.prevPosition);
						data.rotation = it->second.angle;
						_particles->start(it->second.particleSystem, data);
						it->second.prevPosition = p;
					}
				}
				else if (it->second.emitType == TET_FRAMES) {
					++it->second.frameCounter;
					if (it->second.frameCounter >= it->second.frames) {
						it->second.frameCounter = 0;
						ParticleGeneratorData data(it->second.prevPosition);
						data.rotation = it->second.angle;
						_particles->start(it->second.particleSystem, data);
						it->second.prevPosition = p;
					}
				}
				else if (it->second.emitType == TET_TIME) {
					it->second.timer += dt;
					if (it->second.timer >= it->second.time) {
						it->second.timer = 0.0f;
						ParticleGeneratorData data(it->second.prevPosition);
						data.rotation = it->second.angle;
						_particles->start(it->second.particleSystem, data);
						it->second.prevPosition = p;
					}
				}
				++it;
			}
			else {
				it = _pieces.erase(it);
			}
		}
		PR_END("Trail:tick")
	}

}