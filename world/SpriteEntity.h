#pragma once
#include "World.h"

namespace ds {

class SpriteEntity;

class SpriteBehavior {

public:
	SpriteBehavior() : m_Entity(0) , m_Active(true) {}
	virtual ~SpriteBehavior() {}
	void setEntity(SpriteEntity* entity) {
		m_Entity = entity;
	}
	virtual void update(float elapsed) = 0;
	void setActive(bool active) {
		m_Active = active;
	}
	const bool isActive() const {
		return m_Active;
	}
protected:
	SpriteEntity* m_Entity;
private:	
	bool m_Active;
};
// -------------------------------------------------------
// SpriteEntity
// -------------------------------------------------------
class SpriteEntity : public Entity {

typedef std::vector<SpriteBehavior*> Behaviors;

public:
	SpriteEntity() : Entity() , m_Rotation(0.0f) , m_BatchItemID(-1) , m_Sprite(0) , m_UseColor(false) , m_Color(Color::WHITE) , m_Created(false) , m_UseScale(false) , m_ScaleX(1.0f) , m_ScaleY(1.0f) {}
	virtual ~SpriteEntity() {
		if ( m_Created ) {
			delete m_Sprite;
		}		
	}
	void init(int batchID,Sprite* sprite,bool created = false) {
		m_BatchItemID = batchID;
		m_Sprite = sprite;
		m_Position = sprite->position;
		m_Created = created;		
	}
	const EntityType getType() const {
		return ET_SPRITE;
	}
	const int getBatchItemID() const {
		return m_BatchItemID;
	}
	void setSprite(Sprite* sprite) {
		m_Sprite = sprite;
	}
	Sprite* getSprite() {
		return m_Sprite;
	}
	void update(float elapsed) {
		for ( size_t i = 0; i < m_Behaviors.size(); ++i ) {
			SpriteBehavior* behavior = m_Behaviors[i];
			if ( behavior->isActive() ) {
				behavior->update(elapsed);
			}
		}
	}
	template<class T>
	void addBehavior(T* behavior) {
		behavior->setEntity(this);
		m_Behaviors.push_back(behavior);
	}
	void setRotation(float rotation) {
		m_Rotation = rotation;
	}
	const float getRotation() const {
		return m_Rotation;
	}
	void setColor(const Color& color) {
		m_Color = color;
		m_UseColor = true;
	}
	const Color& getColor() const {
		return m_Color;
	}
	const bool useColor() const {
		return m_UseColor;
	}
	void setScale(float sx,float sy) {
		m_UseScale = true;
		m_ScaleX = sx;
		m_ScaleY = sy;
	}
	const bool useScale() const {
		return m_UseScale;
	}
	const float getScaleX() const {
		return m_ScaleX;
	}
	const float getScaleY() const {
		return m_ScaleY;
	}
	void draw() {}
private:
	Behaviors m_Behaviors;
	bool m_UseScale;
	float m_ScaleX;
	float m_ScaleY;
	bool m_Created;
	Sprite* m_Sprite;
	float m_Rotation;
	int m_BatchItemID;
	bool m_UseColor;
	Color m_Color;
};

}
