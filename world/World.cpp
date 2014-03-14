#include "World.h"
#include "..\renderer\shader.h"
#include <assert.h>
#include "..\utils\FileUtils.h"
#include "..\utils\Profiler.h"
#include "..\particles\BoxEmitter.h"

#include "..\utils\StringUtils.h"
#include "..\io\FileWatcher.h"

namespace ds {

World::World() : m_Counter(0) , m_Paused(false) {
	for ( int i = 0; i < 16; ++i ) {
		Layer* layer = &m_Layers[i];
		layer->id = i;
		layer->active = true;
		m_Camera[i] = 0;
		m_RenderTargets[i].texture = -1;
	}
}

World::~World(void) {
	for ( size_t i = 0; i < m_BatchItems.size(); ++i ) {
		delete m_BatchItems[i].spriteBatch;
	}	
}

void World::init(Renderer* renderer) {
	m_Renderer = renderer;
}

// -------------------------------------------------------
// Create render target
// -------------------------------------------------------
int World::createRenderTarget(int layer) {
	RTSetting* rts = &m_RenderTargets[layer];
	sprintf(rts->name,"RT%d",layer);
	rts->texture = m_Renderer->createRenderTarget(rts->name);
	return rts->texture;
}

// -------------------------------------------------------
// Create render target
// -------------------------------------------------------
int World::createRenderTarget(int layer,float width,float height) {
	RTSetting* rts = &m_RenderTargets[layer];
	sprintf(rts->name,"RT%d",layer);
	rts->texture = m_Renderer->createRenderTarget(rts->name,width,height);
	return rts->texture;
}
// -------------------------------------------------------
// Create sprite batch
// -------------------------------------------------------
int World::createSpriteBatch(int textureID,float textureWidth,float textureHeight,int blendState) {
	int idx = m_BatchItems.size();
	SpriteBatchItem sbi;
	sbi.textureID = textureID;	
	//sbi.shaderID = shader::createPTCShader(m_Renderer,textureID);
	sbi.spriteBatch = new SpriteBatch(m_Renderer,1024,textureID);
	if ( blendState != -1 ) {
		sbi.spriteBatch->setBlendState(blendState);
	}
	//sbi.spriteBatch->createMaterial(textureID);
	//LOGC(logINFO,"World") << "Created SpriteBatch - texture: " << sbi.texture << " shader: " << sbi.shaderID;
	m_BatchItems.push_back(sbi);
	return idx;
}

// -------------------------------------------------------
// Set SpriteBatch Shader
// -------------------------------------------------------
void World::setSpriteBatchShader(int batchID,int shaderID) {
	m_BatchItems[batchID].spriteBatch->setShader(shaderID);
}

// -------------------------------------------------------
// Create sprite batch
// -------------------------------------------------------
int World::createSpriteBatch(const char* textureName) {
	int idx = m_BatchItems.size();
	createSpriteBatch(idx,textureName);
	return idx;
}

// -------------------------------------------------------
// Create sprite batch
// -------------------------------------------------------
void World::createSpriteBatch(int idx,const char* textureName,int maxQuads) {
	LOGC(logINFO,"World") << "creating sprite batch - id: " << idx << " texture " << textureName << " maxQuads: " << maxQuads;
	SpriteBatchItem sbi;
	int texture = m_Renderer->getTextureId(textureName);
	if ( texture == -1 ) {
		texture = m_Renderer->loadTexture(textureName);
	}
	assert( texture != -1 );	
	sbi.spriteBatch = new SpriteBatch(m_Renderer,maxQuads,texture);
	sbi.textureID = texture;
	m_BatchItems.push_back(sbi);
}

// -------------------------------------------------------
// Add TextEntity
// -------------------------------------------------------
void World::addTextEntity(int layer,int batchID,const char* fontName,TextEntity* textEntity) {
	//add(layer,textEntity);
	//textEntity->init(m_Renderer,m_BatchItems[batchID].spriteBatch,fontName,getTextureID(batchID),batchID);
}

// -------------------------------------------------------
// Add HUDEntity
// -------------------------------------------------------
void World::addHUDEntity(int layer,HUDEntity* entity,int textureID,const char* fontName) {
	//add(layer,entity);
	//entity->init(m_Renderer,textureID,fontName);
}
/*
// -------------------------------------------------------
// Add SpriteEntity
// -------------------------------------------------------
void World::addSpriteEntity(int layer,int batchID,SpriteEntity* entity,int x,int y,const Rect& textureRect,float rotation,float scaleX,float scaleY,const Color& color) {
	Sprite* sprite = BM_NEW(Sprite);
	sprite->position = Vector2f(x,y);
	sprite->textureRect = textureRect;
	sprite->rotation = rotation;
	sprite->scaleX = scaleX;
	sprite->scaleY = scaleY;
	sprite->color = color;
	add(layer,entity);
	entity->init(batchID,sprite);
}
// -------------------------------------------------------
// Add SpriteEntity
// -------------------------------------------------------
void World::addSpriteEntity(int layer,int batchID,SpriteEntity* entity,const char* name) {
	SpritePrefab* prefab = getPrefab(name);
	assert(prefab != 0);	
	add(layer,entity);
	entity->init(batchID,prefab->sprite);
}

// -------------------------------------------------------
// Add SpriteEntity
// -------------------------------------------------------
void World::addSpriteEntity(int layer,int batchID,SpriteEntity* entity,Sprite* sprite) {	
	assert(sprite != 0);
	add(layer,entity);
	entity->init(batchID,sprite);
}

// -------------------------------------------------------
// Add SpriteEntity
// -------------------------------------------------------
void World::addSpriteEntity(int layer,int batchID,SpriteEntity* entity,int x,int y,Sprite* sprite) {	
	add(layer,entity);
	entity->init(batchID,sprite);
	entity->setPosition(Vector2f(x,y));
}
*/
// -------------------------------------------------------
// Add CollisionEntity
// -------------------------------------------------------
void World::addCollisionEntity(int layer,CollisionEntity* entity) {
	add(layer,entity);
}

// -------------------------------------------------------
// Add entity
// -------------------------------------------------------
void World::add(int layer,Entity* entity) {
	Layer* lyr = &m_Layers[layer];
	entity->setID(m_Counter);
	entity->setLayer(layer);
	lyr->entities.push_back(entity);
	++m_Counter;
}

// -------------------------------------------------------
// Remove entity
// -------------------------------------------------------
void World::remove(Entity* entity) {
	Layer* layer = &m_Layers[entity->getLayer()];
	std::vector<Entity*>::iterator it = layer->entities.begin();
	while ( it != layer->entities.end()) {
		if ( (*it)->getID() == entity->getID() ) {
			m_CollisionManager.remove((*it));
			it = layer->entities.erase(it);
		}
		else {
			++it;
		}
	}
}

// -------------------------------------------------------
// SetCircleShape
// -------------------------------------------------------
void World::setCircleShape(Entity* entity,float radius,int type) {
	entity->setCollisionID(m_CollisionManager.addCircle(entity,radius,type));
}

// -------------------------------------------------------
// SetBoxShape
// -------------------------------------------------------
void World::setBoxShape(Entity* entity,float width,float height,int type) {
	entity->setCollisionID(m_CollisionManager.addBox(entity,width,height,type));
}
// -------------------------------------------------------
// Update
// -------------------------------------------------------
void World::update(float elapsed) {
	PR_START("World-update")
	m_SettingsManager.checkSettingsFiles();
	
	PR_START("World-update-Entities")
	for ( int i = 0; i < 16; ++i ) {
		Layer* layer = &m_Layers[i];		
		for ( size_t j = 0; j < layer->entities.size(); ++j ) {
			Entity* e = layer->entities[j];
			if ( e->isActive() ) {
				e->update(elapsed);
			}
		}
	}
	PR_END("World-update-Entities")
	PR_START("World-update-CollisionCheck")
	m_CollisionManager.reset();
	m_CollisionManager.checkIntersections();
	PR_END("World-update-CollisionCheck")
	PR_END("World-update")
}

// -------------------------------------------------------
// Stop SpriteBatch
// -------------------------------------------------------
void World::stopSpriteBatch() {
	if ( m_CurrentBatch != 0 ) {
		m_CurrentBatch->spriteBatch->end();
		m_CurrentBatch = 0;				
	}
	m_CurrentBatchItem = -1;
}
// -------------------------------------------------------
// Draw
// -------------------------------------------------------
void World::draw() {
	PR_START("World-Draw")
	m_Renderer->set2DCameraOn();
	EntityType currentType = ET_NONE;
	m_CurrentBatchItem = -1;
	int currentBlendState = m_Renderer->getCurrentBlendState();
	m_CurrentBatch = 0;
	//SpriteBatchItem* item = 0;
	//LOG(logINFO) << "----------- DRAW -----------------";
	for ( int i = 0; i < 16; ++i ) {
		Layer* layer = &m_Layers[i];	
		if ( m_RenderTargets[i].texture != -1 ) {	
			stopSpriteBatch();
			RTSetting* rts = &m_RenderTargets[i];
			m_Renderer->setRenderTarget(rts->name);
		}
		for ( size_t j = 0; j < layer->entities.size(); ++j ) {
			Entity* e = layer->entities[j];
			if ( e->isActive() ) {
				if (currentType != e->getType() ) {
					if ( currentType == ET_SPRITE && m_CurrentBatch != 0 ) {		
						stopSpriteBatch();
					}
					currentType = e->getType();
				}
				if ( e->getType() == ET_SPRITE ) {
					/*
					SpriteEntity* se = static_cast<SpriteEntity*>(e);
					assert( se->getSprite() != 0);
					if ( se->getBatchItemID() != m_CurrentBatchItem ) {			
						stopSpriteBatch();
						m_CurrentBatchItem = se->getBatchItemID();
						m_CurrentBatch = &m_BatchItems[m_CurrentBatchItem];
						m_CurrentBatch->spriteBatch->begin();					
					}				
					Sprite* sp = se->getSprite();
					Vector2f pos = e->getPosition();
					if ( m_Camera[i] != 0 ) {
						pos = m_Camera[i]->transform(pos);
					}
					float rotation = sp->rotation;
					if ( se->getRotation() != 0.0f ) {
						rotation = se->getRotation();
					}
					Color cl = sp->color;
					if ( se->useColor() ) {
						cl = se->getColor();
					}
					float sx = sp->scaleX;
					float sy = sp->scaleY;
					if ( se->useScale() ) {
						sx = se->getScaleX();
						sy = se->getScaleY();
					}
					m_CurrentBatch->spriteBatch->draw(pos,sp->textureRect,rotation,sx,sy,cl);
					*/
	
				}
				
				else if ( e->getType() == ET_PARTICLE ) {
					ParticlesystemEntity* pse = static_cast<ParticlesystemEntity*>(e);
					if ( pse->isAlive() ) {
						//m_Renderer->setTexture(pse->getTextureID(),0);
						int currentBS = m_Renderer->getCurrentBlendState();
						if ( pse->getBlendState() != -1 ) {
							m_Renderer->setBlendState(pse->getBlendState());
						}
						pse->draw();
						m_Renderer->setBlendState(currentBS);
					}
				}
				else if ( e->getType() == ET_NEW_PARTICLE ) {
					NewParticlesystemEntity* pse = static_cast<NewParticlesystemEntity*>(e);
					if ( pse->isAlive() ) {
						//m_Renderer->setTexture(pse->getTextureID(),0);
						int currentBS = m_Renderer->getCurrentBlendState();
						if ( pse->getBlendState() != -1 ) {
							m_Renderer->setBlendState(pse->getBlendState());
						}
						pse->draw();
						m_Renderer->setBlendState(currentBS);
					}					
				}				
				else if ( e->getType() == ET_CUSTOM ) {
					e->draw();
				}
			}
		}
		if ( m_RenderTargets[i].texture != -1 ) {
			stopSpriteBatch();
			RTSetting* rts = &m_RenderTargets[i];
			m_Renderer->restoreBackBuffer(rts->name);
		}
	}
	stopSpriteBatch();
	PR_END("World-Draw")
	//m_Renderer->set2DCameraOff();
}

// -------------------------------------------------------
// Debug
// -------------------------------------------------------
void World::debug() {
	for ( int i = 0; i < 16; ++i ) {
		Layer* layer = &m_Layers[i];	
		LOG(logINFO) << "Layer: " << i;
		LOG(logINFO) << "  Active: " << layer->active;
		LOG(logINFO) << "  Rendertarget: " << m_RenderTargets[i].texture;
		LOG(logINFO) << "  Entities: " << layer->entities.size();
		int sprites = 0;
		for ( size_t j = 0; j < layer->entities.size(); ++j ) {
			Entity* e = layer->entities[j];
			if ( e->isActive() ) {
				if ( e->getType() == ET_PARTICLE ) {
					ParticlesystemEntity* pse = static_cast<ParticlesystemEntity*>(e);
					LOG(logINFO) << "    Particles: " << pse->numParticles() << " / " << pse->getMaxParticles();
				}
				else if ( e->getType() == ET_NEW_PARTICLE ) {
					NewParticlesystemEntity* pse = static_cast<NewParticlesystemEntity*>(e);
					LOG(logINFO) << "    New Particles: " << pse->numParticles() << " / " << pse->getMaxParticles();
				}
				else if ( e->getType() == ET_SPRITE ) {
					++sprites;
				}
			}
		}
		LOG(logINFO) << "    Active sprites: " << sprites;
	}
	LOG(logINFO) << "---- CollisionManager -----";
	m_CollisionManager.debug();

}

// -------------------------------------------------------
// Add new ParticleSystemEntity
// -------------------------------------------------------
void World::addNewParticleSystemEntity(int layer,int textureID,const char* fileName,NewParticlesystemEntity* entity,int maxParticles,int blendState) {
	// FIXME: check if file exists!!
	add(layer,entity);
	NewParticleSystem* particleSystem = new NewParticleSystem(m_Renderer,maxParticles,textureID,blendState);	
	particleSystem->load(fileName);
	if ( m_Camera[layer] != 0 ) {
		particleSystem->setCamera(m_Camera[layer]);
	}
	entity->init(particleSystem,textureID,blendState);
}
// -------------------------------------------------------
// Add Particle Entity
// -------------------------------------------------------
void World::addParticleSystemEntity(int layer,int textureID,const char* dirName,ParticlesystemEntity* entity,int maxParticles,int blendState) {
	add(layer,entity);
	int quads = maxParticles;
	ParticleSystem* particleSystem = new ParticleSystem(m_Renderer,quads,textureID,blendState);
	ParticleData* particleData = new ParticleData;
	char dirBuffer[256];
	sprintf(dirBuffer,"content\\resources\\settings\\%s",dirName);
	FSDirectory dir(dirBuffer);
	dir.list();
	char buffer[256];
	// first we need particle data
	for ( int i = 0; i < dir.numFiles(); ++i ) {
		if ( dir.getFileNameNoEnding(i) == "particle_data") {
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			//m_SettingsManager.loadSettings(buffer,particleData,false);
			particleSystem->setParticleData(particleData);
		}
	}

	for ( int i = 0; i < dir.numFiles(); ++i ) {				
		if ( dir.getFileNameNoEnding(i) == "step_size") {
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			m_SettingsManager.loadSettings(buffer,particleSystem->createSizePath(),false);
		}
		if ( dir.getFileNameNoEnding(i) == "step_color") {
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			m_SettingsManager.loadSettings(buffer,particleSystem->getColorPath(),false);
		}
		if ( dir.getFileNameNoEnding(i) == "radial_velocity") {
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			m_SettingsManager.loadSettings(buffer,particleSystem->createRadialVelocityPath(),false);
		}	
		/*
		if ( dir.getFileNameNoEnding(i) == "wiggle") {
			WiggleAffectorSettings* settings = new WiggleAffectorSettings;
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			m_SettingsManager.loadSettings(buffer,settings);
			particleSystem->addParticleAffector(new WiggleAffector("wiggle",settings));
		}
		*/
		if ( dir.getFileNameNoEnding(i) == "rotation") {
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			m_SettingsManager.loadSettings(buffer,particleSystem->createRotationPath(),false);
		}	
		if ( dir.getFileNameNoEnding(i) == "ring_emitter") {
			ParticleEmitterData* emitterData = new ParticleEmitterData;
			sprintf(buffer,"%s\\emitter_data",dirName);
			//m_SettingsManager.loadSettings(buffer,emitterData);
			RingEmitterSettings* emitterSettings = new RingEmitterSettings;
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			m_SettingsManager.loadSettings(buffer,emitterSettings);
			RingEmitter* emitter = new RingEmitter(emitterData,emitterSettings,particleData);
			particleSystem->setEmitter(emitter);
		}	
		if ( dir.getFileNameNoEnding(i) == "box_emitter") {
			ParticleEmitterData* emitterData = new ParticleEmitterData;
			sprintf(buffer,"%s\\emitter_data",dirName);
			//m_SettingsManager.loadSettings(buffer,emitterData);
			BoxEmitterSettings* emitterSettings = new BoxEmitterSettings;
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			//m_SettingsManager.loadSettings(buffer,emitterSettings);
			BoxEmitter* emitter = new BoxEmitter(emitterData,emitterSettings,particleData);
			particleSystem->setEmitter(emitter);
		}	
		if ( dir.getFileNameNoEnding(i) == "cone_emitter") {
			ParticleEmitterData* emitterData = new ParticleEmitterData;
			sprintf(buffer,"%s\\emitter_data",dirName);
			//m_SettingsManager.loadSettings(buffer,emitterData,false);
			ConeEmitterSettings* emitterSettings = new ConeEmitterSettings;
			sprintf(buffer,"%s\\%s",dirName,dir.getFileNameNoEnding(i).c_str());
			//m_SettingsManager.loadSettings(buffer,emitterSettings,false);
			ConeEmitter* emitter = new ConeEmitter(emitterData,emitterSettings,particleData);
			LOG(logINFO) << "Setting cone emitter";
			particleSystem->setEmitter(emitter);
		}	
	}
	if ( m_Camera[layer] != 0 ) {
		particleSystem->setCamera(m_Camera[layer]);
	}
	entity->init(particleSystem,textureID,blendState);

}

bool World::loadData(const char* name) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",name);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "spritebatch" ) {
				/*
				"texture" : "Textures" ,
				"id" : "0" ,
				"size_x" : "1024" ,
				"size_y" : "1024"
				*/
				int id = c->getInt(0,"id");
				// FIXME: check if we already have one with this id
				std::string texture = c->getProperty("texture");
				createSpriteBatch(id,texture.c_str());
			}
			else if ( c->getName() == "sprite" ) {
				LOG(logINFO) << "Creating sprite " << c->getProperty("name");
			}
		}
		gFileWatcher->registerFile(buffer,this);
		return true;
	}
	return false;
}
/*
bool World::loadHUD(const char* name,HUDEntity* hudEntity) {	
	char buffer[256];
	sprintf(buffer,"content\\resources\\%s.json",name);
	JSONReader reader;
	if ( reader.parse("content\\resources\\hud.json") ) {
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "hud" ) {
				int layer = c->getInt(0,"layer");
				int textureID = c->getInt(0,"texture_id");
				std::string fontName = c->getProperty("font_name");
				addHUDEntity(layer,hudEntity,textureID,fontName.c_str());
				hudEntity->load(name);
			}	
		}		
		return true;
	}
	return false;
}
*/
void World::reload(const char* fileName) {
	LOG(logINFO) << "----- RELOADING -----";
	JSONReader reader;
	if ( reader.parse(fileName) ) {
		std::vector<Category*> categories = reader.getCategories();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "sprite" ) {				
			}
		}
	}
}

}