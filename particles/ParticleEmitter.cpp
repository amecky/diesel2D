#pragma once
#include "ParticleEmitter.h"

namespace ds {
	/*
	// -------------------------------------------------------
	// ParticleEmitter
	// -------------------------------------------------------
	ParticleEmitter::ParticleEmitter() {
		//add(new DefaultParticleGenerator());
	}

	// -------------------------------------------------------
	// ParticleEmitter - destructor
	// -------------------------------------------------------
	ParticleEmitter::~ParticleEmitter() {
		clear(false);
	}

	//void ParticleEmitter::createInstance(ParticleEmitterInstance* instance) {

	//}

	// -------------------------------------------------------
	// clear
	// -------------------------------------------------------
	void ParticleEmitter::clear(bool addDefault) {		
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
	void ParticleEmitter::generate(ParticleArray* array, const v3& pos, float dt, uint32* start, uint32* end, bool started) {
		if (m_Active) {
			if (m_Burst) {
				generate(array, pos, m_EmitterData.count, start, end, dt);
				m_Active = false;
			}
			else {
				m_Accu += m_Frequency;
				if (m_Accu >= m_Frequency) {
					int count = (int)m_Accu;
					m_Accu -= count;
					generate(array, pos, count, start, end, dt);
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
	void ParticleEmitter::generate(ParticleArray* array, const v3& pos, int count, uint32* start, uint32* end, float dt) {
		*start = array->countAlive;
		*end = *start + count;
		//uint32 end = start + started;
		if (*end > array->count) {
			*end = array->count;
		}		
		//for (size_t i = 0; i < m_Generators.size(); ++i) {
			//m_Generators[i]->generate(array, data, dt, *start, *end);
		//}
		for (uint32 i = *start; i < *end; ++i) {
			array->color[i] = Color::WHITE;
			array->scale[i] = Vector2f(1, 1);
			array->rotation[i] = 0.0f;
			array->timer[i] = Vector3f(0, 1, 1);
			array->random[i] = 1.0f;
			array->acceleration[i] = Vector3f(0, 0, 0);
			array->velocity[i] = Vector3f(0, 0, 0);
			array->position[i] = pos;
			array->type[i] = 1;
		}
		for (uint32 i = *start; i < *end; ++i) {
			array->wake(i);
		}		
	}

	// -------------------------------------------------------
	// get generator
	// -------------------------------------------------------
	//ParticleGenerator* ParticleEmitter::getGenerator(ParticleGeneratorType type) {
		//for (size_t i = 0; i < m_Generators.size(); ++i) {
			//if (m_Generators[i]->getType() == type) {
				//return m_Generators[i];
			//}
		//}
		//return 0;
	//}

	// -------------------------------------------------------
	// add generator
	// -------------------------------------------------------
	//void ParticleEmitter::add(ParticleGenerator* generator) {
		//m_Generators.push_back(generator);
	//}

	// -------------------------------------------------------
	// get emitter data
	// -------------------------------------------------------
	ParticleEmitterData& ParticleEmitter::getEmitterData() {
		return m_EmitterData;
	}

	// -------------------------------------------------------
	// tick
	// -------------------------------------------------------
	void ParticleEmitter::tick(float dt) {
		if (m_Active) {
			m_Timer += dt;
			// emitt particles
			if (m_Timer >= m_EmitterData.duration) {
				m_Active = false;
			}
		}
	}

	void ParticleEmitter::init() {
		float total = static_cast<float>(m_EmitterData.count);
		LOG << "--> total: " << total;
		LOG << "--> duration: " << m_EmitterData.duration;
		if (m_EmitterData.duration == 0.0f) {
			m_EmitterData.burst = true;
			m_EmitterData.endless = false;
			m_EmitterData.frequency = 0.0f;
		}
		else if (m_EmitterData.duration < 0.0f) {
			m_EmitterData.endless = true;
			m_EmitterData.burst = false;
			m_EmitterData.frequency = total / 60.0f;
		}
		else {
			//m_Frequency = total / (m_EmitterData.duration * 60.0f);
			m_EmitterData.frequency = total / 60.0f;
			m_EmitterData.burst = false;
			m_EmitterData.endless = false;
		}
		LOG << "--> burst: " << m_EmitterData.burst;
		LOG << "--> endless: " << m_EmitterData.endless;
		LOG << "--> frequency: " << m_EmitterData.frequency;
	}
	*/
}


