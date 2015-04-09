#pragma once
#include "ParticleEmitter.h"

namespace ds {

	void ParticleSpawner::tick(float dt) {

	}

	int ParticleSpawner::spawn(ParticleArray* array) {
		uint32 start = array->countAlive;
		uint32 end = start + 100;// m_EmitterData.count;
		if (end > array->count) {
			end = array->count;
		}
		return end - start;
	}


	// -------------------------------------------------------
	// ParticleEmitter
	// -------------------------------------------------------
	ParticleEmitter::ParticleEmitter() {
		add(new DefaultParticleGenerator());
	}

	// -------------------------------------------------------
	// ParticleEmitter - destructor
	// -------------------------------------------------------
	ParticleEmitter::~ParticleEmitter() {
		clear(false);
	}

	void ParticleEmitter::createInstance(ParticleEmitterInstance* instance) {

	}

	// -------------------------------------------------------
	// clear
	// -------------------------------------------------------
	void ParticleEmitter::clear(bool addDefault) {
		Generators::iterator it = m_Generators.begin();
		while (it != m_Generators.end()) {
			delete (*it);
			it = m_Generators.erase(it);
		}
		if (addDefault) {
			add(new DefaultParticleGenerator());
		}
		m_Active = false;
		m_Timer = 0.0f;
		m_Accu = 0.0f;
	}

	void ParticleEmitter::start() {
		m_Active = true;
		m_Timer = 0.0f;
	}

	void ParticleEmitter::stop() {
		m_Active = false;
		m_Timer = 0.0f;
	}
	// -------------------------------------------------------
	// generate
	// -------------------------------------------------------
	void ParticleEmitter::generate(ParticleArray* array, const Vector2f& pos, float dt, bool started) {
		if (m_Active) {
			if (m_Burst) {
				generate(array, pos, m_EmitterData.count, dt);
				m_Active = false;
			}
			else {
				m_Accu += m_Frequency;
				if (m_Accu >= m_Frequency) {
					int start = (int)m_Accu;
					m_Accu -= start;
					generate(array, pos, start, dt);
				}
				if (!m_Endless) {
					if (m_Timer >= m_EmitterData.duration) {
						m_Active = false;
					}
					m_Timer += dt;
				}
			}
		}
	}

	// -------------------------------------------------------
	// generate
	// -------------------------------------------------------
	void ParticleEmitter::generate(ParticleArray* array, const Vector2f& pos,int count, float dt) {
		uint32 start = array->countAlive;
		uint32 end = start + count;
		//uint32 end = start + started;
		if (end > array->count) {
			end = array->count;
		}

		for (size_t i = 0; i < m_Generators.size(); ++i) {
			m_Generators[i]->generate(array, pos, dt, start, end);
		}
		for (uint32 i = start; i < end; ++i) {
			array->wake(i);
		}		
	}

	// -------------------------------------------------------
	// get generator
	// -------------------------------------------------------
	ParticleGenerator* ParticleEmitter::getGenerator(ParticleGeneratorType type) {
		for (size_t i = 0; i < m_Generators.size(); ++i) {
			if (m_Generators[i]->getType() == type) {
				return m_Generators[i];
			}
		}
		return 0;
	}

	// -------------------------------------------------------
	// add generator
	// -------------------------------------------------------
	void ParticleEmitter::add(ParticleGenerator* generator) {
		m_Generators.push_back(generator);
	}

	// -------------------------------------------------------
	// get emitter data
	// -------------------------------------------------------
	ParticleEmitterData& ParticleEmitter::getEmitterData() {
		return m_EmitterData;
	}

	// -------------------------------------------------------
	// get particle spawner
	// -------------------------------------------------------
	ParticleSpawner& ParticleEmitter::getParticleSpawner() {
		return m_Spawner;
	}

	// -------------------------------------------------------
	// tick
	// -------------------------------------------------------
	void ParticleEmitter::tick(float dt) {
		if (m_Active) {
			m_Timer += dt;
			// emitt particles
			if (m_Timer >= m_Duration) {
				m_Active = false;
			}
		}
	}

	void ParticleEmitter::init() {
		float total = static_cast<float>(m_EmitterData.count);
		LOG << "--> total: " << total;
		LOG << "--> duration: " << m_EmitterData.duration;
		if (m_EmitterData.duration == 0.0f) {
			m_Burst = true;
			m_Endless = false;
		}
		else if (m_EmitterData.duration < 0.0f) {
			m_Endless = true;
			m_Burst = false;
			m_Frequency = total / 60.0f;
		}
		else {
			//m_Frequency = total / (m_EmitterData.duration * 60.0f);
			m_Frequency = total / 60.0f;
			m_Burst = false;
			m_Endless = false;
		}
		LOG << "--> burst: " << m_Burst;
		LOG << "--> endless: " << m_Endless;
		LOG << "--> frequency: " << m_Frequency;
	}
}


