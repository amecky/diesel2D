//-------------------------------------------------------------------------
//
// File:   World.h
// Desc:   The 2d world.
// Author: amecky <amecky@gamil.com>
//
//-------------------------------------------------------------------------
#pragma once
#include <vector>
#include "..\nodes\SpriteBatch.h"
#include "..\renderer\Camera2D.h"
//#include "animations\Animation.h"
#include "..\particles\ParticleSystem.h"
#include "..\particles\NewParticleSystem.h"
#include "..\settings\SettingsManager.h"
#include "..\base\EventStream.h"
#include "..\renderer\render_types.h"
#include "..\renderer\shader.h"
#include "CollisionManager.h"
#include "..\io\Serializer.h"
#include "..\memory\DataBlockAllocator.h"

namespace ds {
// -------------------------------------------------------
// EntityType
// -------------------------------------------------------
enum EntityType {
	ET_NONE,
	ET_SPRITE,
	ET_PARTICLE,
	ET_NEW_PARTICLE,
	ET_HUD,
	ET_OVERLAY,
	ET_TEXT,
	ET_COLLISION,
	ET_CUSTOM
};

// -------------------------------------------------------
//! The basic entity virtual base class
/*! This is the base of all entity subclasses. It provides
	already common methods 
*/
// -------------------------------------------------------
class Entity {

public:
	Entity() : m_ID(-1) , m_Layer(0) , m_Active(true) , m_ColID(-1) {}
	virtual ~Entity() {}
	void setID(int id) {
		m_ID = id;
	}
	const int getID() const {
		return m_ID;
	}
	//! Sets the layer that this entity belongs to
	/*
		\param layer the layer
	*/
	void setLayer(int layer) {
		m_Layer = layer;
	}
	//! Returns the layer that this entity belongs to
	/*
		\return the layer
	*/
	const int getLayer() const {
		return m_Layer;
	}
	//! Sets the state of the entity to be active or not
	/*
		\param active the new state of the entity
	*/
	void setActive(bool active) {
		m_Active = active;
	}
	//! Returns the state of this entity whether it is active or not
	/*
		\return true if entity is active
	*/
	const bool isActive() const {
		return m_Active;
	}
	
	virtual void update(float elapsed) = 0;
	virtual void draw() = 0;
	virtual const EntityType getType() const = 0;

	void setPosition(const Vec2& pos) {
		m_Position = pos;
	}
	const Vec2& getPosition() const {
		return m_Position;
	}	
	void setCollisionID(int colID) {
		m_ColID = colID;
	}
	const int getCollisionID() const {
		return m_ColID;
	}
protected:
	Vec2 m_Position;
private:
	int m_ColID;
	bool m_Active;
	int m_ID;
	int m_Layer;
};

// -------------------------------------------------------
// CollisionEntity
// -------------------------------------------------------
class CollisionEntity : public Entity {

public:
	CollisionEntity() : Entity() {}
	virtual ~CollisionEntity() {}
	const EntityType getType() const {
		return ET_COLLISION;
	}
	void update(float elapsed) {}
	void draw() {}
};
// -------------------------------------------------------
// ScreenOverlayEntity
// -------------------------------------------------------
class ScreenOverlayEntity : public Entity {

public:
	ScreenOverlayEntity() : Entity() , m_TextureID(-1) , m_Shader(-1) {}
	virtual ~ScreenOverlayEntity() {}
	void init(Renderer* renderer,int textureID,int shaderID = -1) {
		m_Renderer = renderer;
		m_TextureID = textureID;
		if ( shaderID == -1 ) {
			m_Shader = shader::createPTCShader(renderer,textureID);
		}
		else {
			m_Shader = shaderID;
		}

		m_Buffer = new PTCBuffer(renderer,12,20,false);	
		m_Buffer->clear();
		float width = static_cast<float>(renderer->getWidth());
		float height = static_cast<float>(renderer->getHeight());

		m_Buffer->add(PTCVertex(Vec3(-0.5f * width, 0.5f * height,0.0f),Vec2(0.0f,0.0f)));
		m_Buffer->add(PTCVertex( 0.5f * width, 0.5f * height,0.0f,1.0f,0.0f));
		m_Buffer->add(PTCVertex( 0.5f * width,-0.5f * height,0.0f,1.0f,1.0f));
		m_Buffer->add(PTCVertex(-0.5f * width,-0.5f * height,0.0f,0.0f,1.0f));	

		m_Buffer->addIndex(0);
		m_Buffer->addIndex(1);
		m_Buffer->addIndex(2);
		m_Buffer->addIndex(2);
		m_Buffer->addIndex(3);
		m_Buffer->addIndex(0);

		m_Buffer->update();
	}
	void update(float elapsed) {}
	 const EntityType getType() const {
		 return ET_OVERLAY;
	 }
	 void draw() {
		 //DrawCounter* drawCounter = m_Renderer->getDrawCounter();
		 //m_DrawCounter->addDrawCall();
		 //m_DrawCounter->addPrimitives(handle->vBufferRef.count);
		 //m_DrawCounter->addIndices(handle->iBufferRef.count);
		 m_Renderer->setTexture(m_TextureID,0);
		 if ( m_Shader != -1 ) {
			 Shader* shader = &m_Renderer->getShader(m_Shader);			 
			 uint32 numPasses = m_Renderer->startShader(shader);
			 for ( UINT p = 0; p < numPasses; ++p ) {		
				 HR(shader->m_FX->BeginPass(p));
				 m_Renderer->setShaderParameter(shader);
				 m_Buffer->prepareData();	 
				 m_Buffer->render(2);
				 HR(shader->m_FX->EndPass());
			 }
			 m_Renderer->endShader(shader);
		 }
		 else {
			m_Renderer->setTexture(m_TextureID,0);
			 m_Buffer->prepareData();	 
			 m_Buffer->render(2);
		 }
	 }
	 const int getTextureID() const {
		 return m_TextureID;
	 }
private:
	Renderer* m_Renderer;
	int m_Shader;
	int m_TextureID;
	PTCBuffer* m_Buffer;
};


// -------------------------------------------------------
// ParticlesystemEntity
// -------------------------------------------------------
class ParticlesystemEntity : public Entity {

public:
	ParticlesystemEntity() : Entity() , m_ParticleSystem(0) {}
	virtual ~ParticlesystemEntity() {
		if ( m_ParticleSystem != 0 ) {
			delete m_ParticleSystem;
		}
	}
	void init(ParticleSystem* particleSystem,int textureID,int blendState = -1) {
		m_ParticleSystem = particleSystem;
		m_TextureID = textureID;
		m_BlendState = blendState;
	}
	void update(float elapsed) {
		m_ParticleSystem->update(elapsed);
	}
	void draw() {
		m_ParticleSystem->draw();
	}
	const EntityType getType() const {
		return ET_PARTICLE;
	}
	const int getBlendState() const {
		return m_BlendState;
	}
	void start(const Vec2& pos) {
		m_ParticleSystem->resetEmitterCounter();
		m_ParticleSystem->setEmitterPosition(pos);
		m_ParticleSystem->emitParticles();
	}
	void setEmitterPosition(const Vec2& emitterPos) {
		m_ParticleSystem->setEmitterPosition(emitterPos);
	}
	template<class T>
	T* getEmitter() {
		return m_ParticleSystem->getEmitter<T>();
	}
	void stop() {
		m_ParticleSystem->stop();
	}
	const bool isAlive() const {
		return m_ParticleSystem->isActive();
	}
	const int getTextureID() const {
		return m_TextureID;
	}
	const uint32 numParticles() const {
		return m_ParticleSystem->numParticles();
	}
	const uint32 getMaxParticles() const {
		return m_ParticleSystem->getMaxParticles();
	}
private:
	int m_TextureID;
	int m_BlendState;
	ParticleSystem* m_ParticleSystem;
};


// -------------------------------------------------------
// ParticlesystemEntity
// -------------------------------------------------------
class NewParticlesystemEntity : public Entity {

public:
	NewParticlesystemEntity() : Entity() , m_ParticleSystem(0) {}
	virtual ~NewParticlesystemEntity() {
		if ( m_ParticleSystem != 0 ) {
			delete m_ParticleSystem;
		}
	}
	void init(NewParticleSystem* particleSystem,int textureID,int blendState = -1) {
		m_ParticleSystem = particleSystem;
		m_TextureID = textureID;
		m_BlendState = blendState;
	}
	void update(float elapsed) {
		m_ParticleSystem->update(elapsed);
	}
	void draw() {
		m_ParticleSystem->draw();
	}
	const EntityType getType() const {
		return ET_NEW_PARTICLE;
	}
	const int getBlendState() const {
		return m_BlendState;
	}
	void start(const Vec2& pos) {
		//m_ParticleSystem->resetEmitterCounter();
		//m_ParticleSystem->setEmitterPosition(pos);
		m_ParticleSystem->start(pos);
	}
	void setEmitterPosition(const Vec2& emitterPos) {
		m_ParticleSystem->setPosition(emitterPos);
	}
	template<class T>
	T* getEmitter() {
		return m_ParticleSystem->getEmitter<T>();
	}
	void stop() {
		//m_ParticleSystem->stop();
	}
	const bool isAlive() const {
		return m_ParticleSystem->isActive();
	}
	const int getTextureID() const {
		return m_TextureID;
	}
	const uint32 numParticles() const {
		return m_ParticleSystem->numParticles();
	}
	const uint32 getMaxParticles() const {
		return m_ParticleSystem->getMaxParticles();
	}
	NewParticleSystem* getParticleSystem() {
		return m_ParticleSystem;
	}
private:
	int m_TextureID;
	int m_BlendState;
	NewParticleSystem* m_ParticleSystem;
};

// -------------------------------------------------------
// Layer
// -------------------------------------------------------
struct Layer {
	int id;
	bool active;
	std::vector<Entity*> entities;	
};

// -------------------------------------------------------
// SpriteBatchItem
// -------------------------------------------------------
struct SpriteBatchItem {
	SpriteBatch* spriteBatch;
	int textureID;
};


class World;
// -------------------------------------------------------
// GameObject
// -------------------------------------------------------
class GameObject {

public:
	GameObject() {}
	virtual ~GameObject() {}
	void setWorld(World* world) {
		m_World = world;
	}
	void setRenderer(Renderer* renderer) {
		m_Renderer = renderer;
	}
	virtual void update(float elapsed) = 0;
	virtual void init() = 0;
	const bool isActive() const {
		return m_Active;
	}
	void setActive(bool active) {
		m_Active = active;
	}
	EventStream& getEvents() {
		return m_Events;
	}
	void resetEvents() {
		m_Events.reset();
	}
	const bool hasEvents() const {
		return m_Events.num() > 0;
	}
protected:
	World* m_World;
	EventStream m_Events;
	Renderer* m_Renderer;
private:
	GameObject(const GameObject& other) {}
	bool m_Active;
};
// -------------------------------------------------------
// World
// -------------------------------------------------------
class HUDEntity;
class TextEntity;
class SpriteEntity;

struct SpritePrefab : public Gizmo {

	Sprite* sprite;
	IdString name;

	SpritePrefab() : Gizmo("SpritePrefab") {
		sprite = BM_NEW(Sprite);
		add("position",&sprite->position);
		add("texture",&sprite->textureRect);
		add("scale_x",&sprite->scaleX);
		add("scale_y",&sprite->scaleY);
		add("rotation",&sprite->rotation);
		add("color",&sprite->color);
	}

};

class World : public Serializer {

struct RTSetting {

	int layer;
	int texture;
	char name[10];
};

typedef std::vector<GameObject*> GameObjects;
//typedef std::vector<Sprite*> Sprites;
typedef std::vector<SpritePrefab*> SpritePrefabs;

public:
	World(void);
	~World(void);
	void init(Renderer* renderer);
	void setCamera(int layer,Camera2D* camera) {
		m_Camera[layer] = camera;
	}
	// FIXME: loadSprites(const char* fileName); oder loadSprites();
	// create
	int createRenderTarget(int layer);
	int createRenderTarget(int layer,float width,float height);
	void createSpriteBatch(int id,const char* textureName,int maxQuads = 1024);
	int createSpriteBatch(const char* textureName);
	int createSpriteBatch(int textureID,float textureWidth = 1024.0f,float textureHeight = 1024.0f,int blendState = -1);
	void setSpriteBatchShader(int batchID,int shaderID);
	// add entity
	void add(int layer,Entity* entity);
	// FIXME: addSpriteEntity(layer,batch,entity,const char* spriteName);
	void addSpriteEntity(int layer,int batchID,SpriteEntity* entity,Sprite* sprite);
	void addSpriteEntity(int layer,int batchID,SpriteEntity* entity,int x,int y,Sprite* sprite);
	void addSpriteEntity(int layer,int batchID,SpriteEntity* entity,const char* name);
	void addSpriteEntity(int layer,int batchID,SpriteEntity* entity,int x,int y,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE);

	void addParticleSystemEntity(int layer,int textureID,const char* dirName,ParticlesystemEntity* entity,int maxParticles,int blendState = -1);
	void addNewParticleSystemEntity(int layer,int textureID,const char* fileName,NewParticlesystemEntity* entity,int maxParticles,int blendState = -1);
	void addHUDEntity(int layer,HUDEntity* entity,int textureID,const char* fontName);
	void addTextEntity(int layer,int batchID,const char* fontName,TextEntity* textEntity);
	//
	void remove(Entity* entity);
	// collision 
	void addCollisionEntity(int layer,CollisionEntity* entity);
	void setCircleShape(Entity* entity,float radius,int type);
	void setBoxShape(Entity* entity,float width,float height,int type);
	bool containsCollisions() {
		return m_CollisionManager.getCollisionCounter() > 0;
	}
	int getCollisionCounter() {
		return m_CollisionManager.getCollisionCounter();
	}
	void getCollision(int idx,Collision& co) {
		m_CollisionManager.getCollision(idx,co);
	}
	void addCollisionIgnore(int firstType,int secondType) {
		m_CollisionManager.addIgnore(firstType,secondType);
	}

	int getTextureID(int batchID) {
		return m_BatchItems[batchID].textureID;
	}
	void draw();
	void update(float elapsed);	
	void loadSettings(const char* name,DynamicSettings* settings) {		
		m_SettingsManager.loadSettings(name,settings);
	}
	DynamicSettings* getSettings(const char* name) {
		return m_SettingsManager.getSettings(name);
	}
	void loadSpriteSettings(const char* name,Sprite* sprite) {
		if ( m_SettingsManager.hasSpriteSettings(name)) {
			sprite = m_SettingsManager.getSpriteSettings(name);
		}
		else {
			sprite = m_SettingsManager.loadSpriteSettings(name);
		}
	}
	void togglePause() {
		m_Paused = !m_Paused;
	}
	template<class S>
	void createGameObject(S* obj);

	bool loadData(const char* name);
	bool loadHUD(const char* fileName,HUDEntity* hudEntity);
	void debug();

	Sprite* getSprite(const char* name) {
		IdString hash = string::murmur_hash(name);
		for ( size_t i = 0; i < m_SpritePrefabs.size(); ++i ) {
			if ( m_SpritePrefabs[i]->name == hash ) {
				return m_SpritePrefabs[i]->sprite;
			}
		}
		return 0;
	}
	SpritePrefab* getPrefab(const char* name) {
		IdString hash = string::murmur_hash(name);
		for ( size_t i = 0; i < m_SpritePrefabs.size(); ++i ) {
			if ( m_SpritePrefabs[i]->name == hash ) {
				return m_SpritePrefabs[i];
			}
		}
		return 0;
	}
private:
	
	void toggleSpriteBatch();
	void stopSpriteBatch();
	// serializable
	virtual void reload(const char* fileName);

	CollisionManager m_CollisionManager;
	GameObjects m_GameObjects;
	Camera2D* m_Camera[16];
	int m_CurrentBatchItem;
	std::vector<SpriteBatchItem> m_BatchItems;
	SpriteBatchItem* m_CurrentBatch;
	uint32 m_Counter;
	Renderer* m_Renderer;
	Layer m_Layers[16];
	SettingsManager m_SettingsManager;
	bool m_Paused;
	RTSetting m_RenderTargets[16];

	// FIXME: move to resource manager
	SpritePrefabs m_SpritePrefabs;
};

template<class S>
void World::createGameObject(S* obj) {
	obj->setWorld(this);
	obj->setRenderer(m_Renderer);
	obj->init();
	obj->GameObject::setActive(true);
	m_GameObjects.push_back(obj);
}

}