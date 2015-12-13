#include "ParticleSystem.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\utils\Profiler.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include <stdarg.h>

namespace ds {

	const float VP_ARRAY[] = {
		-0.5f, 0.5f, 0.5f, 0.5f,
		0.5f, -0.5f, -0.5f, -0.5f
	};

	void NewParticleSystem::start(const ParticleGeneratorData& data) {
		_generatorData = data;
		PR_START("NPS:emitterStart");
		m_Emitter.start();
		PR_END("NPS:emitterStart");
		PR_START("NPS:emitterGenerate");
		uint32 start = 0;
		uint32 end = 0;
		m_Emitter.generate(&m_Array, data, 0.0f,&start, &end);
		//LOG << "---- start: " << start << " end: " << end;
		PR_END("NPS:emitterGenerate");
		PR_START("NPS:emitterModifiers");
		for (size_t i = 0; i < m_Modifiers.size(); ++i) {
			m_Modifiers[i]->init(&m_Array, start, end);
		}
		PR_END("NPS:emitterModifiers");
	}

	void NewParticleSystem::start(const Vector3f& startPosition) {
		ParticleGeneratorData data(startPosition);
		start(data);
	}

	ParticleModifierData* NewParticleSystem::getData(const char* modifierName) {
		for (int i = 0; i < _count_modifiers; ++i) {
			const ModifierInstance& instance = _modifier_instances[i];
			if (strcmp(instance.modifier->getName(), modifierName) == 0) {
				return instance.data;
			}
		}
		return 0;
	}

	void NewParticleSystem::update(float elapsed) {
		PR_START("NPS:update");
		uint32 start = 0;
		uint32 end = 0;
		m_Emitter.generate(&m_Array, _generatorData, elapsed, &start, &end);
		for (size_t i = 0; i < m_Modifiers.size(); ++i) {
			m_Modifiers[i]->update(&m_Array, elapsed);
		}

		for (int i = 0; i < _count_modifiers; ++i) {
			const ModifierInstance& instance = _modifier_instances[i];
			instance.modifier->update(&m_Array, instance.data, elapsed);
		}
		PR_END("NPS:update");
	}

	void NewParticleSystem::render() {
		PR_START("NPS:render");
		if (m_Array.countAlive > 0) {
			ds::sprites::draw(m_Data.texture, m_Array);
			renderer::drawCounter().particles += m_Array.countAlive;
		}
		PR_END("NPS:render");
	}

	void NewParticleSystem::clear() {
		Modifiers::iterator it = m_Modifiers.begin();
		while (it != m_Modifiers.end()) {
			delete (*it);
			it = m_Modifiers.erase(it);
		}
		m_Emitter.clear();
	}

	void NewParticleSystem::removeModifierByName(const char* name) {
		Modifiers::iterator it = m_Modifiers.begin();
		while (it != m_Modifiers.end()) {
			if (strcmp((*it)->getName(),name) == 0) {
				delete (*it);
				it = m_Modifiers.erase(it);
			}
			else {
				++it;
			}
		}
	}

	ParticleModifier* NewParticleSystem::getModifier(ParticleModifierType type) {
		for (size_t i = 0; i < m_Modifiers.size(); ++i) {
			if (m_Modifiers[i]->getType() == type) {
				return m_Modifiers[i];
			}
		}
		return 0;
	}

	bool NewParticleSystem::exportData(JSONWriter& writer) {
		return true;
	}

	bool NewParticleSystem::importData(JSONReader& reader) {
		clear();
		Category* root = reader.getCategory("particlesystem");
		if (root != 0) {
			_id = root->getUInt32("id", 0);
			m_Data.id = _id;
			m_Data.textureID = root->getUInt32("texture_id", 0);
			m_Data.textureRect = root->getRect("texture_rect");

			Category* modifiers = root->getChild("modifiers");
			if (modifiers != 0) {
				std::vector<Category*> categories = modifiers->getChildren();
				for (size_t i = 0; i < categories.size(); ++i) {
					Category* c = categories[i];
					if (_factory->addModifier(this, c->getName().c_str())) {
						ParticleModifierData* data = getData(c->getName().c_str());
						data->read(c);
					}
					//ParticleModifier* modifier = modifier::create_by_name(c->getName().c_str());
					//if (modifier != 0) {
						//modifier->read(c);
						//addModifier(modifier);
					//}
					else {
						LOGE << "cannot find modifier: " << c->getName();
					}
				}
			}

			Category* emitter = root->getChild("emitter");
			if (emitter != 0) {
				std::vector<Category*> categories = emitter->getChildren();
				ParticleEmitterData& data = m_Emitter.getEmitterData();
				data.count = emitter->getUInt32("count", 10);
				data.ejectionPeriod = emitter->getUInt32("ejection_period", 0);
				data.ejectionVariance = emitter->getUInt32("ejection_variance", 0);
				data.ejectionCounter = emitter->getUInt32("ejection_counter", 0);
				data.duration = emitter->getFloat("duration", 0.0f);
				for (size_t i = 0; i < categories.size(); ++i) {
					Category* c = categories[i];
					ParticleGenerator* generator = generator::create_by_name(c->getName().c_str());
					if (generator != 0) {
						generator->read(c);
						addGenerator(generator);
					}
					else {
						LOGE << "cannot find generator: " << c->getName();
					}
				}
			}
		}
		return true;
	}

	bool NewParticleSystem::saveData(BinaryWriter& writer) {
		for (int i = 0; i < _count_modifiers; ++i) {
			writer.startChunk(1, 1);
			const ModifierInstance& instance = _modifier_instances[i];
			writer.write(instance.modifier->getChunkID());
			writer.closeChunk();
		}
		return true;
	}

	bool NewParticleSystem::loadData(BinaryLoader& loader) {
		clear();
		while (loader.openChunk() == 0) {
			if (loader.getChunkID() >= 100 && loader.getChunkID() < 200) {
				ParticleModifier* modifier = createModifier(loader.getChunkID());
				if (modifier != 0) {
					modifier->load(&loader);
					addModifier(modifier);
				}
			}
			else if (loader.getChunkID() >= 200 && loader.getChunkID() < 300) {
				ParticleGenerator* generator = createGenerator(loader.getChunkID());
				if (generator != 0) {
					generator->load(&loader);
					addGenerator(generator);
				}
			}
			else if (loader.getChunkID() == 300) {
				ParticleEmitterData& data = m_Emitter.getEmitterData();
				loader.read(&data.count);
				loader.read(&data.ejectionPeriod);
				loader.read(&data.ejectionVariance);
				loader.read(&data.ejectionCounter);
				loader.read(&data.duration);
				m_Emitter.init();
			}
			else if (loader.getChunkID() == 400) {
				loader.read(&m_Data.id);
				loader.read(&m_Data.textureID);
				loader.read(&m_Data.textureRect);
				m_Data.texture = ds::math::buildTexture(m_Data.textureRect.top, m_Data.textureRect.left, m_Data.textureRect.width(), m_Data.textureRect.height());
			}
			loader.closeChunk();
		}
		prepareVertices();
		return true;
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
		PR_START("NPS:buildVertices");
		Vector3f p(0, 0 , 0);
		Vector3f dp(0, 0, 0);
		for (uint32 i = 0; i < m_Array.countAlive; ++i) {
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
		PR_END("NPS:buildVertices");
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
