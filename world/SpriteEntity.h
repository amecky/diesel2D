#pragma once
#include "World.h"

namespace ds {


// -------------------------------------------------------
// SpriteEntity
// -------------------------------------------------------
class SpriteEntity : public Entity {

	//typedef std::vector<BaseAnimation*> Animations;

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
		//for ( size_t i = 0; i < m_Animations.size(); ++i ) {
		//m_Animations[i]->update(elapsed);
		//m_Animations[i]->updateSprite(m_Sprite);
		//}
	}
	//void addAnimation(BaseAnimation* animation) {
	//m_Animations.push_back(animation);
	//}
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
	//Animations m_Animations;
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
