#include "ParticleSystem.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\utils\Profiler.h"
#include <stdarg.h>

namespace ds {

	const float VP_ARRAY[] = {
		-0.5f, 0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f, -0.5f
	};

	void lifeOverTime(const ModifierData& data, ParticleArray* array, float dt) {
		LiveOverTimeData myData;
		if (data.get(100, &myData, sizeof(LiveOverTimeData))) {
			for (uint32 i = 0; i < array->countAlive; ++i) {
				array->timer[i].x += dt;
				array->timer[i].y = array->timer[i].x / array->timer[i].z;
			}
			uint32 cnt = 0;
			while (cnt < array->countAlive) {
				if (array->timer[cnt].x > array->timer[cnt].z) {
					array->kill(cnt);
				}
				++cnt;
			}
		}
	}

	void move(const ModifierData& data, ParticleArray* array, float dt) {
		for (uint32 i = 0; i < array->countAlive; ++i) {
			array->velocity[i] += array->acceleration[i] * dt;
			array->position[i] += array->velocity[i] * dt;
		}
	}

	void NewParticleSystem::start(const ParticleGeneratorData& data) {
		_generatorData = data;
		m_Emitter.start();
		m_Emitter.generate(&m_Array, data, 0.0f);
		for (size_t i = 0; i < m_Modifiers.size(); ++i) {
			m_Modifiers[i]->update(&m_Array, 0.0f);
		}
	}

	void NewParticleSystem::start(const Vector3f& startPosition) {
		ParticleGeneratorData data(startPosition);
		start(data);
	}

	void NewParticleSystem::setDebugName(const char* name) {
		strncpy(m_DebugName,name,32);
	}

	void NewParticleSystem::update(float elapsed) {
		PR_START("NPS:update")
		m_Emitter.generate(&m_Array, _generatorData, elapsed);
		for (size_t i = 0; i < m_Modifiers.size(); ++i) {
			m_Modifiers[i]->update(&m_Array, elapsed);
		}
		//buildVertices();
		PR_END("NPS:update")
	}

	void NewParticleSystem::render() {
		PR_START("NPS:render")
		if (m_Array.countAlive > 0) {
			ds::sprites::draw(m_Data.texture, m_Array);
			renderer::drawCounter().particles += m_Array.countAlive;
		}
		PR_END("NPS:render")
	}

	void NewParticleSystem::clear() {
		Modifiers::iterator it = m_Modifiers.begin();
		while (it != m_Modifiers.end()) {
			delete (*it);
			it = m_Modifiers.erase(it);
		}
		m_Emitter.clear();
	}

	void NewParticleSystem::load(BinaryLoader* loader) {	
		clear();
		while ( loader->openChunk() == 0 ) {	
			if (loader->getChunkID() >= 100 && loader->getChunkID() < 200) {
				ParticleModifier* modifier = createModifier(loader->getChunkID());
				if (modifier != 0) {
					modifier->load(loader);
					addModifier(modifier);
				}
			}
			else if (loader->getChunkID() >= 200 && loader->getChunkID() < 300) {
				ParticleGenerator* generator = createGenerator(loader->getChunkID());
				if (generator != 0) {
					generator->load(loader);
					addGenerator(generator);
				}
			}
			else if ( loader->getChunkID() == 300 ) {
				ParticleEmitterData& data = m_Emitter.getEmitterData();
				loader->read(&data.count);
				loader->read(&data.ejectionPeriod);
				loader->read(&data.ejectionVariance);
				loader->read(&data.ejectionCounter);
				loader->read(&data.duration);
				m_Emitter.init();
			}
			else if ( loader->getChunkID() == 400 ) {
				loader->read(&m_Data.id);
				loader->read(&m_Data.textureID);
				loader->read(&m_Data.textureRect);
				m_Data.texture = ds::math::buildTexture(m_Data.textureRect.top, m_Data.textureRect.left, m_Data.textureRect.width(), m_Data.textureRect.height());
			}
			loader->closeChunk();
		}
		prepareVertices();
	}

	void NewParticleSystem::prepareVertices() {
		for (int i = 0; i < MAX_PARTICLES; ++i) {
			m_Array.vertices[i* 4].uv.x = m_Data.texture.uv.x;
			m_Array.vertices[i * 4].uv.y = m_Data.texture.uv.y;
			m_Array.vertices[i * 4 + 1].uv.x = m_Data.texture.uv.z;
			m_Array.vertices[i * 4 + 1].uv.y = m_Data.texture.uv.y;
			m_Array.vertices[i * 4 + 2].uv.x = m_Data.texture.uv.z;
			m_Array.vertices[i * 4 + 2].uv.y = m_Data.texture.uv.w;
			m_Array.vertices[i * 4 + 3].uv.x = m_Data.texture.uv.x;
			m_Array.vertices[i * 4 + 3].uv.y = m_Data.texture.uv.w;
		}
	}

	void NewParticleSystem::buildVertices() {
		PR_START("NPS:buildVertices")
		Vector3f p(0, 0 , 0);
		Vector3f dp(0, 0, 0);
		for (int i = 0; i < m_Array.countAlive; ++i) {
			Vector3f cor = m_Array.position[i];
			//cor = cor - ds::renderer::getSelectedViewport().getPosition();
		
			for (int j = 0; j < 4; ++j) {
				p.x = VP_ARRAY[j * 2] * m_Data.texture.dim.x;
				p.y = VP_ARRAY[j * 2 + 1] * m_Data.texture.dim.y;
				//vector::srt(cor, p, m_Array.scale[i], m_Array.rotation[i], &dp);
				dp = vector::srt(cor, p, m_Array.scale[i].x, m_Array.scale[i].y, m_Array.rotation[i]);
				m_Array.vertices[i * 4 + j].x = dp.x;
				m_Array.vertices[i * 4 + j].y = dp.y;
				m_Array.vertices[i * 4 + j].z = dp.z;
				m_Array.vertices[i * 4 + j].color = m_Array.color[i];
			}
		}
		PR_END("NPS:buildVertices")
	}

	ParticleGenerator* NewParticleSystem::createGenerator(int id) {
		switch (id) {
			case 200: return new RingGenerator; break;
			case 201: return new ParticleRandomGenerator; break;
			case 202: return new LifetimeGenerator; break;
			case 203: return  new RadialVelocityGenerator; break;
			case 204: return new SizeGenerator; break;
			case 205: return new ColorGenerator; break;
			case 206: return new HSVColorGenerator; break;
			case 207: return new PointGenerator; break;
			case 208: return new CircleGenerator; break;
			case 209: return new LineGenerator; break;
			case 210: return new VelocityGenerator; break;
			case 211: return new SphereGenerator; break;
			case 212: return new RandomSphereGenerator; break;
			default: return 0;
		}
	}

	ParticleModifier* NewParticleSystem::createModifier(int id) {
		switch (id) {
			case 100: return new ParticleTimeModifier; break;
			case 101: return new ParticlePositionModifier; break;
			case 102: return new LinearColorModifier; break;
			case 103: return new LinearSizeModifier; break;
			case 104: return new PerpendicularMoveModifier; break;
			case 105: return new ColorPathModifier; break;
			case 106: return new DampingVelocityModifier; break;
			case 107: return new SizePathModifier; break;
			case 108: return new VelocityRotationModifier; break;
			case 109: return new AlphaPathModifier; break;
			default: return 0;
		}
	}

}
