#include "ParticleSystem.h"
#include "..\io\FileWatcher.h"
#include "..\compiler\Converter.h"
#include "..\utils\Profiler.h"

namespace ds {

void ParticleSystem::init() {	
	m_Emitter = 0;
	m_Count = 0;
	m_RadialVelocityPath = 0;
	m_Rotating = false;
	m_SizePath = 0;
	m_UseWiggle = false;
	m_Camera = 0;
	m_ColorPath = new ColorPath;
	m_EmitFrequency = 0.0f;
	m_EmitDelay = 0.0f;
	m_EmitTimer = 0.0f;
	m_Particles.resize(m_MaxParticles);
}


ParticleSystem::~ParticleSystem(void) {
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
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void ParticleSystem::update(float elapsed) {
	PR_START("ParticleSystem::update")
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
		}
		if ( cnt > 0 ) {			
			start(m_Position,cnt);
		}
	}
	
	Vector2f size(0,0);
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
		Vector2f velocity = p->normal * radialVelocity;
		if ( m_Rotating ) {
			m_RotationPath.data.get(norm,&rotationSpeed);
			if ( p->random > 0.0f ) {				
				p->rotation -= elapsed * DEGTORAD(rotationSpeed) * p->random;
			}
			else if ( p->random < 0.0f ) {
				p->rotation += elapsed * DEGTORAD(rotationSpeed) * p->random;
			}
			else {
				p->rotation = DEGTORAD(rotationSpeed);
			}
		}
		// move particle		
		if ( m_UseWiggle ) {
			Vector2f perp = Vector2f(-velocity.y,velocity.x);
			perp = normalize(perp);
			perp = perp * (sin(norm * m_WiggleSettings.amplitude) * m_WiggleSettings.radius);// * p->random;	
			velocity += perp;
		}
		p->position += velocity * elapsed;
		if ( m_UseTrail && p->head ) {
			float dist = distance(p->position,p->prevPos);
			if ( dist >= m_TrailSettings.distance ) {
				buildTrailParticle(p);
				p->prevPos = p->position;
			}
		}
	}
	PR_END("ParticleSystem::update")
}

void ParticleSystem::buildTrailParticle(Particle* baseParticle) {
	Particle* p = &m_Particles[m_Count++];
	p->head = false;
	p->rotationSpeed = baseParticle->rotationSpeed;
	p->rotation = baseParticle->rotation;
	p->radialVelocity = baseParticle->radialVelocity;
	p->ttl = baseParticle->ttl;	
	p->initialSize = baseParticle->initialSize;
	p->random = baseParticle->random;	
	p->position = baseParticle->position;
	p->prevPos = p->position;
	p->timer = 0.0f;
}

// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void ParticleSystem::render() {
	PR_START("ParticleSystem::render")
	if ( m_Count > 0 ) {		
		Color color = Color::WHITE;
		Vector2f size(0,0);	
		for ( uint32 i = 0; i < m_Count; ++i ) {
			Particle* p = &m_Particles[i];
			Vector2f pp = p->position;
			if ( m_Camera != 0 ) {
				pp = m_Camera->transform(pp);
			}
			float sizeX = p->initialSize.x;
			float sizeY = p->initialSize.y;
			float norm = p->timer / p->ttl;
			m_ColorPath->update(norm,&color);	
			if ( m_SizePath != 0 ) {
				m_SizePath->get(norm,&size);
				sizeX *= size.x;
				sizeY *= size.y;
			}
			m_Renderer->draw(pp,m_TextureID,m_ParticleData->textureRect,p->rotation,sizeX,sizeY,color);
		}
	}
	PR_END("ParticleSystem::render")
}

// -------------------------------------------------------
// Start
// -------------------------------------------------------
void ParticleSystem::start(const Vector2f& startPos,int forcedCount) {
	if ( m_Emitter != 0 ) {
		m_Position = startPos;
		int total = m_EmitterData->count;
		bool rnd = false;
		if ( forcedCount != -1 ) {
			total = forcedCount;
			rnd = true;
		}
		for ( int i = 0; i < total; ++i ) {
			if ( m_Count < m_MaxParticles ) {
				Particle* p = &m_Particles[m_Count++];
				p->head = true;
				p->rotationSpeed = math::random(m_ParticleData->minRotationSpeed,m_ParticleData->maxRotationSpeed);
				p->rotation = 0.0f;		
				p->radialVelocity = 1.0f;
				p->ttl = m_EmitterData->ttl + math::random(-m_EmitterData->ttlVariance,m_EmitterData->ttlVariance);
				float rndSize = 1.0f;
				if ( m_EmitterData->sizeMinVariance != 0.0f && m_EmitterData->sizeMaxVariance != 0.0f ) {
					rndSize = math::random(m_EmitterData->sizeMinVariance,m_EmitterData->sizeMaxVariance);
				}
				p->initialSize.x = m_ParticleData->initialSize.x * rndSize;
				p->initialSize.y = m_ParticleData->initialSize.y * rndSize;
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
				float angle = 0.0f;
				//m_Emitter->getPoint(i,total,p->position,p->normal,rnd);			
				m_Emitter->getPoint(i,m_EmitterData->count,p->position,p->normal,&angle,rnd);			
				p->position += startPos;
				p->prevPos = p->position;
				//p->rotation = math::getTargetAngle(startPos,p->position);
				p->rotation = angle;//math::getTargetAngle(p->normal,Vector2f(1,0));
				p->timer = 0.0f;
			}
		}
	}
}

// -------------------------------------------------------
// Set ParticleData
// -------------------------------------------------------
void ParticleSystem::setParticleData(ParticleData* particleData) {
	m_ParticleData = particleData;
	LOGC("ParticleSystem") << "Setting particle data";
	m_ColorPath->reset();
	m_ColorPath->add(0.0f,particleData->startColor);
	m_ColorPath->add(1.0f,particleData->endColor);	
}

// -------------------------------------------------------
// Set EmitterData
// -------------------------------------------------------
void ParticleSystem::setEmitterData(ParticleEmitterData* emitterData) {
	m_EmitterData = emitterData;
	if ( m_EmitterData->ejectionCounter > 0 ) {
		m_EmitFrequency = static_cast<float>(m_EmitterData->ejectionCounter);
		m_EmitTimer = 0.0f;
		m_EmitDelay = 1.0f / m_EmitFrequency;		
	}
	LOGC("ParticleSystem") << "setting emitter data";
	LOGC("ParticleSystem") << "freq " << m_EmitFrequency << " delay " << m_EmitDelay;
}

void ParticleSystem::clear() {
	m_Rotating = false;
	m_UseWiggle = false;
	m_UseTrail = false;
	m_EmitFrequency = 0.0f;
	m_EmitDelay = 0.0f;
	m_EmitTimer = 0.0f;
	if ( m_Emitter != 0 ) {
		delete m_Emitter;
	}
}

void ParticleSystem::load(BinaryLoader* loader) {
	clear();
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == CHNK_PARTICLE_DATA ) {	
			m_ParticleData->load(loader);			
			setParticleData(m_ParticleData);
		}
		else if ( loader->getChunkID() == CHNK_EMITTER_DATA ) {							
			m_EmitterData->load(loader);
			setEmitterData(m_EmitterData);
		}	
		else if ( loader->getChunkID() == CHNK_RING_EMITTER ) {				
			RingEmitterSettings* emitterSettings = new RingEmitterSettings;
			emitterSettings->load(loader);
			RingEmitterMesh* emitter = new RingEmitterMesh(emitterSettings);
			setEmitter(emitter);			
		}	
		else if ( loader->getChunkID() == CHNK_BOX_EMITTER ) {	
			BoxEmitterSettings* emitterSettings = new BoxEmitterSettings;
			emitterSettings->load(loader);
			BoxEmitterMesh* emitter = new BoxEmitterMesh(emitterSettings);
			setEmitter(emitter);			
		}	
		else if ( loader->getChunkID() == CHNK_PARTICLE_SIZE ) {			
			int total = 0;
			loader->read(&total);
			createSizePath();			
			float timeStep = 0.0f;
			Vector2f pos;
			for ( size_t i = 0; i < total; ++i ) {
				loader->read(&timeStep);
				loader->read(&pos);
				m_SizePath->add(timeStep,pos);
			}
		}	
		else if ( loader->getChunkID() == CHNK_PARTICLE_COLOR ) {			
			int total = 0;
			loader->read(&total);
			m_ColorPath->reset();
			float timeStep = 0.0f;
			Color clr = Color::WHITE;			
			for ( size_t i = 0; i < total; ++i ) {
				loader->read(&timeStep);
				loader->read(&clr);				
				m_ColorPath->add(timeStep,clr);
			}				
		}	
		else if ( loader->getChunkID() == CHNK_PARTICLE_VELOCITY ) {
			createRadialVelocityPath();
			int total = 0;
			loader->read(&total);
			float timeStep = 0.0f;
			float value = 1.0f;
			for ( int i = 0; i < total; ++i ) {
				loader->read(&timeStep);
				loader->read(&value);
				m_RadialVelocityPath->add(timeStep,value);				
			}	
		}
		else if ( loader->getChunkID() == CHNK_PARTICLE_ROTATION ) {
			m_Rotating = true;
			m_RotationPath.load(loader);				
		}
		loader->closeChunk();
	}		
}
/*
void ParticleSystem::load(const char* fileName) {
	ParticleData* particleData = new ParticleData;
	ParticleEmitterData* emitterData = new ParticleEmitterData;	
	m_Rotating = false;
	m_UseWiggle = false;
	m_UseTrail = false;
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",fileName);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		LOG << "Particle file found";
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
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "size") {			
				createSizePath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					m_SizePath->add(timeStep,c->getVector2f(propertyNames[i]));
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
			if ( c->getName() == "trail" ) {
				m_UseTrail = true;
				m_TrailSettings.load(c);
			}
			if ( c->getName() == "rotation") {	
				m_Rotating = true;
				m_RotationPath.load(c);				
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
		gFileWatcher->registerFile(buffer,this);
	}
}

void ParticleSystem::reload(const char* fileName) {
	JSONReader reader;
	if ( reader.parse(fileName) ) {
		m_Rotating = false;
		m_UseWiggle = false;
		m_UseTrail = false;
		LOG << "Reloading particle file " << fileName;
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
				setEmitterData(m_EmitterData);
			}		
			if ( c->getName() == "size") {	
				createSizePath();
				std::vector<std::string> propertyNames;
				c->getPropertyNames(propertyNames);
				for ( size_t i = 0; i < propertyNames.size(); ++i ) {
					float timeStep = std::stof(propertyNames[i].c_str());
					m_SizePath->add(timeStep,c->getVector2f(propertyNames[i]));
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
			if ( c->getName() == "trail" ) {
				m_UseTrail = true;
				m_TrailSettings.load(c);
			}
			if ( c->getName() == "rotation") {				
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
*/
}
