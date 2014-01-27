#include "NewParticleSystem.h"
#include "..\io\FileWatcher.h"

namespace ds {

NewParticleSystem::NewParticleSystem(Renderer* renderer,uint32 maxParticles,int textureID,int blendState) 
	: m_MaxParticles(maxParticles) , m_Emitter(0) , m_Count(0) , m_RadialVelocityPath(0) , m_Rotating(false) , m_SizePath(0) , m_UseWiggle(false) {

	m_SpriteBatch = new SpriteBatch(renderer,maxParticles,textureID);
	if ( blendState != -1 ) {
		m_SpriteBatch->setBlendState(blendState);
	}
	m_Camera = 0;
	m_TextureID = textureID;
	m_ColorPath = new ColorPath;
	m_EmitFrequency = 0.0f;
	m_EmitDelay = 0.0f;
	m_EmitTimer = 0.0f;
	m_Particles.resize(maxParticles);
}


NewParticleSystem::~NewParticleSystem(void) {
	if ( m_Emitter != 0 ) {
		delete m_Emitter;
	}
	delete m_ColorPath;

	if ( m_SizePath != 0 ) {
		delete m_SizePath;
	}
	if ( m_RadialVelocityPath != 0 ) {
		delete m_RadialVelocityPath;
	}
	delete m_EmitterData;
	delete m_ParticleData;
	delete m_SpriteBatch;
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void NewParticleSystem::update(float elapsed) {
	int low = 0;
	int high = m_Count - 1;
	bool upwards = true;
	while (high >= low) {
		Particle &p = upwards ? m_Particles[low] : m_Particles[high];
		p.timer += elapsed;
		if (upwards) {
			if (p.timer <= p.ttl) {
				low++;
			}
			else {
				upwards = false;
			}
		} 
		else {
			if (p.timer <= p.ttl) {
				m_Particles[low++] = p;
				upwards = true;
			}
			high--;
		}
	}
	m_Count = high + 1;

	if ( m_EmitFrequency > 0.0f ) {
		m_EmitTimer += elapsed;
		int cnt = 0;
		while ( m_EmitTimer >= m_EmitDelay ) {
			float diff = m_EmitTimer / m_EmitDelay;
			m_EmitTimer -= m_EmitDelay;
			++cnt;
			// create particle
			//LOG(logINFO) << "create particle - diff " << diff;
		}
		if ( cnt > 0 ) {
			start(m_Position,cnt);
		}
	}
	
	Vec2 size(0,0);
	float radialVelocity = 0.0f;
	float rotationSpeed = 0.0f;
	for ( uint32 i = 0; i < m_Count; ++i ) {
		Particle* p = &m_Particles[i];
		float norm = p->timer / p->ttl;
		radialVelocity = p->radialVelocity;		
		if ( m_RadialVelocityPath != 0 ) {
			m_RadialVelocityPath->update(norm,&radialVelocity);
			radialVelocity *= p->radialVelocity;
		}
		Vec2 velocity = p->normal * radialVelocity;
		if ( m_Rotating ) {
			rotationSpeed = m_RotationPath.data.get(norm);
			if ( p->random > 0.0f ) {				
				p->rotation -= elapsed * DEGTORAD(rotationSpeed) * p->random;
			}
			else {
				p->rotation += elapsed * DEGTORAD(rotationSpeed) * p->random;
			}
			//float angle = math::reflect(p->rotation);
			//velocity = math::getRadialVelocity(RADTODEG(angle),radialVelocity);
		}
		// move particle		
		if ( m_UseWiggle ) {
			Vec2 perp = Vec2(-velocity.y,velocity.x);
			perp = vector::normalize(perp);
			perp = perp * (sin(norm * m_WiggleSettings.amplitude) * m_WiggleSettings.radius) * p->random;	
			velocity += perp;
		}
		vector::addScaled(p->position,velocity,elapsed);
	}
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void NewParticleSystem::draw() {
	if ( m_Count > 0 ) {
		m_SpriteBatch->begin();
		Color color = Color::WHITE;
		Vec2 size(0,0);	
		for ( uint32 i = 0; i < m_Count; ++i ) {
			Particle* p = &m_Particles[i];
			Vec2 pp = p->position;
			if ( m_Camera != 0 ) {
				pp = m_Camera->transform(pp);
			}
			float sizeX = p->initialSize.x;
			float sizeY = p->initialSize.y;
			float norm = p->timer / p->ttl;
			m_ColorPath->update(norm,&color);	
			if ( m_SizePath != 0 ) {
				m_SizePath->update(norm,&size);
				sizeX *= size.x;
				sizeY *= size.y;
			}
			m_SpriteBatch->draw(pp,m_ParticleData->textureRect,p->rotation,sizeX,sizeY,color);
		}
		m_SpriteBatch->end();
	}
}

// -------------------------------------------------------
// Start
// -------------------------------------------------------
void NewParticleSystem::start(const Vec2& startPos,int forcedCount) {
	if ( m_Emitter != 0 ) {
		m_Position = startPos;
		int total = m_EmitterData->count;
		if ( forcedCount != -1 ) {
			total = forcedCount;
		}
		for ( int i = 0; i < total; ++i ) {
			if ( m_Count < m_MaxParticles ) {
				Particle* p = &m_Particles[m_Count++];
				p->rotationSpeed = math::random(m_ParticleData->minRotationSpeed,m_ParticleData->maxRotationSpeed);
				p->rotation = 0.0f;		
				p->radialVelocity = 1.0f;
				p->ttl = m_EmitterData->ttl + math::random(-m_EmitterData->ttlVariance,m_EmitterData->ttlVariance);
				p->initialSize.x = m_ParticleData->initialSize.x + math::random(-m_EmitterData->sizeXVariance,m_EmitterData->sizeXVariance);
				p->initialSize.y = m_ParticleData->initialSize.y + math::random(-m_EmitterData->sizeYVariance,m_EmitterData->sizeYVariance);
				float vel = m_EmitterData->velocity;
				if ( m_EmitterData->velocityVariance != 0.0f ) {
					vel += math::random(-m_EmitterData->velocityVariance,m_EmitterData->velocityVariance);
				}		
				p->radialVelocity = vel;
				if ( m_ParticleData->random != 0.0f ) {
					p->random = math::random(-m_ParticleData->random,m_ParticleData->random);
				}
				else {
					p->random = 0.0f;
				}
				m_Emitter->getPoint(i,total,p->position,p->normal);			
				p->position += startPos;
				p->rotation = math::getTargetAngle(startPos,p->position);
				//LOG(logINFO) << "pos " << DBG_V2(p->position) << " normal " << DBG_V2(p->normal);
				p->timer = 0.0f;
			}
		}
	}
}

// -------------------------------------------------------
// Set ParticleData
// -------------------------------------------------------
void NewParticleSystem::setParticleData(ParticleData* particleData) {
	m_ParticleData = particleData;
	LOGC(logINFO,"ParticleSystem") << "Setting particle data";
	m_ColorPath->reset();
	m_ColorPath->add(0.0f,particleData->startColor);
	m_ColorPath->add(1.0f,particleData->endColor);	
}

// -------------------------------------------------------
// Set EmitterData
// -------------------------------------------------------
void NewParticleSystem::setEmitterData(ParticleEmitterData* emitterData) {
	m_EmitterData = emitterData;
	if ( m_EmitterData->ejectionCounter > 0 ) {
		m_EmitFrequency = static_cast<float>(m_EmitterData->ejectionCounter);
		m_EmitTimer = 0.0f;
		m_EmitDelay = 1.0f / m_EmitFrequency;
		LOG(logINFO) << "freq " << m_EmitFrequency << " delay " << m_EmitDelay;
	}
}

void NewParticleSystem::load(const char* fileName) {
	ParticleData* particleData = new ParticleData;
	ParticleEmitterData* emitterData = new ParticleEmitterData;	
	m_Rotating = false;
	m_UseWiggle = false;
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",fileName);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		LOG(logINFO) << "Particle file found";
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "particle_data" ) {
				particleData->load(c);
				setParticleData(particleData);
			}
			if ( c->getName() == "emitter_data" ) {				
				emitterData->load(c);				
				setEmitterData(emitterData);
			}
		}
		gFileWatcher->registerFile(buffer,this);
	}
	if ( reader.parse(buffer) ) {	
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "size") {			
				createSizePath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					m_SizePath->add(timeStep,c->getVec2(propertyNames[i]));
				}
			}
			if ( c->getName() == "color") {
				std::vector<std::string> propertyNames;
				m_ColorPath->reset();
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					Color clr = Color::WHITE;
					c->getColor(propertyNames[i],&clr);
					m_ColorPath->add(timeStep,clr);
				}
			}
			if ( c->getName() == "radial_velocity") {
				createRadialVelocityPath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					float value = 1.0f;
					c->getFloat(propertyNames[i],&value);
					m_RadialVelocityPath->add(timeStep,value);
				}
			}	
			if ( c->getName() == "wiggle" ) {
				m_UseWiggle = true;
				m_WiggleSettings.load(c);
			}
			if ( c->getName() == "rotation") {	
				m_Rotating = true;
				m_RotationPath.load(c);
				/*
				createRotationPath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = atof(propertyNames[i].c_str());
					float value = 1.0f;
					c->getFloat(propertyNames[i],&value);
					m_RotationPath->add(timeStep,value);
				}
				*/

			}	
			if ( c->getName() == "ring_emitter") {			
				RingEmitterSettings* emitterSettings = new RingEmitterSettings;
				emitterSettings->load(c);
				RingEmitterMesh* emitter = new RingEmitterMesh(emitterSettings);
				setEmitter(emitter);
			}		
			if ( c->getName() == "box_emitter") {			
				BoxEmitterSettings* emitterSettings = new BoxEmitterSettings;
				emitterSettings->load(c);
				BoxEmitterMesh* emitter = new BoxEmitterMesh(emitterSettings);
				setEmitter(emitter);
			}	
			if ( c->getName() == "cone_emitter") {			
				ConeEmitterSettings* emitterSettings = new ConeEmitterSettings;
				emitterSettings->load(c);
				ConeEmitterMesh* emitter = new ConeEmitterMesh(emitterSettings);
				setEmitter(emitter);
			}	
			if ( c->getName() == "point_emitter") {						
				PointEmitterMesh* emitter = new PointEmitterMesh();
				setEmitter(emitter);
			}		
		}
	}
}

void NewParticleSystem::reload(const char* fileName) {
	JSONReader reader;
	if ( reader.parse(fileName) ) {
		m_Rotating = false;
		m_UseWiggle = false;
		LOG(logINFO) << "Reloading particle file " << fileName;
		// FIXME: set default colors if not found
		// FIXME: set default size path if not found		
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "particle_data" ) {
				m_ParticleData->load(c);
			}
			if ( c->getName() == "emitter_data" ) {				
				m_EmitterData->load(c);				
			}		
			if ( c->getName() == "size") {	
				createSizePath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					m_SizePath->add(timeStep,c->getVec2(propertyNames[i]));
				}
			}
			if ( c->getName() == "color") {
				m_ColorPath->reset();
				std::vector<std::string> propertyNames;				
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					Color clr = Color::WHITE;
					c->getColor(propertyNames[i],&clr);
					m_ColorPath->add(timeStep,clr);
				}
			}
			if ( c->getName() == "radial_velocity") {
				createRadialVelocityPath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					float value = 1.0f;
					c->getFloat(propertyNames[i],&value);
					m_RadialVelocityPath->add(timeStep,value);
				}
			}	
			if ( c->getName() == "wiggle" ) {
				m_UseWiggle = true;
				m_WiggleSettings.load(c);
			}
			if ( c->getName() == "rotation") {
				/*
				createRotationPath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = atof(propertyNames[i].c_str());
					float value = 1.0f;
					c->getFloat(propertyNames[i],&value);
					m_RotationPath->add(timeStep,value);
				}
				*/
				m_Rotating = true;
				m_RotationPath.load(c);
			}	
			if ( c->getName() == "ring_emitter") {			
				RingEmitterMesh* emitter = getEmitter<RingEmitterMesh>();
				RingEmitterSettings* emitterSettings = emitter->getSettings();
				emitterSettings->load(c);
			}		
			if ( c->getName() == "box_emitter") {			
				BoxEmitterMesh* emitter = getEmitter<BoxEmitterMesh>();
				BoxEmitterSettings* emitterSettings = emitter->getSettings();
				emitterSettings->load(c);
			}	
			if ( c->getName() == "cone_emitter") {			
				ConeEmitterMesh* emitter = getEmitter<ConeEmitterMesh>();
				ConeEmitterSettings* emitterSettings = emitter->getSettings();
				emitterSettings->load(c);
			}	
			if ( c->getName() == "point_emitter") {						
				//PointEmitterMesh* emitter = new PointEmitterMesh();
				//setEmitter(emitter);
			}		
		}
	}
}

}
