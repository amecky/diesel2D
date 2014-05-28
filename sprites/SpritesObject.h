#pragma once
#include "..\base\GameObject.h"
#include "..\utils\Profiler.h"

namespace ds {

// -------------------------------------------------------
// SpritesObject - container of sprites
// -------------------------------------------------------
template<int Size,int CollisionType = -1>
class SpritesObject : public GameObject {

public:
	SpritesObject() : GameObject() {
		for ( int i = 0; i < Size; ++i ) {
			ds::Sprite* sp = &m_Sprites[i];
			sp->setActive(false);
			sp->setID(i);
		}
	}

	virtual ~SpritesObject() {}

	// -------------------------------------------------------
	// Returns next free sprite
	// -------------------------------------------------------
	Sprite* activate() {
		int idx = findFreeSprite();
		if ( idx != -1 ) {
			return &m_Sprites[idx];		
		}
		return 0;
	}

	Sprite* activateByDescription(int descriptionID) {
		int idx = findFreeSprite();
		if ( idx != -1 ) {
			Sprite* sp = &m_Sprites[idx];		
			const SpriteDescription& description = m_Renderer->getSpriteDescription(descriptionID);
			sp->setActive(true);
			sp->setTextureRect(description.textureRect);
			sp->setAngle(description.angle);
			sp->setScale(description.scale);			
			sp->setColor(description.color);
			sp->setIndex(description.index);
			sp->setUserValue(description.userValue);
			sp->setRadius(description.radius);
			sp->setVelocity(description.velocity);
			sp->setPosition(description.position);
			sp->resetTimer();
			return sp;
		}
		return 0;
	}

	// -------------------------------------------------------
	// Returns next free sprite
	// -------------------------------------------------------
	Sprite* activate(const Vector2f& pos,const Rect& textureRect,float rotation = 0.0f,float scaleX = 1.0f,float scaleY = 1.0f,const Color& color = Color::WHITE) {
		int idx = findFreeSprite();
		if ( idx != -1 ) {
			Sprite* sp = &m_Sprites[idx];		
			sp->setActive(true);
			sp->setTextureRect(textureRect);
			sp->setAngle(rotation);
			sp->setScale(scaleX,scaleY);
			sp->setPosition(pos);
			sp->setColor(color);
			return sp;
		}
		return 0;
	}

	// -------------------------------------------------------
	// Get sprite
	// -------------------------------------------------------
	Sprite* get(int index) {
		assert(index < Size);
		return &m_Sprites[index];
	}

	// -------------------------------------------------------
	// Set sprite to not active
	// -------------------------------------------------------
	void deactivate(Sprite* sprite) {
		if ( sprite != 0 ) {
			deactivateByID(sprite->getID());
		}
	}
	// -------------------------------------------------------
	// Remove by ID
	// -------------------------------------------------------
	void deactivateByID(int id) {
		assert(id < Size);
		Sprite* b = &m_Sprites[id];
		b->setActive(false);
	}

	// -------------------------------------------------------
	// Render all active sprites
	// -------------------------------------------------------
	void render() {
		for ( int i = 0; i < Size; ++i ) {
			m_Renderer->draw(m_Sprites[i]);
		}
	}

	// -------------------------------------------------------
	// Update
	// -------------------------------------------------------
	void update(float elapsed) {
		PR_START("SpritesObject::update")
		for ( int i = 0; i < Size; ++i ) {
			Sprite* sp = &m_Sprites[i];
			if ( sp->isActive() ) {
				sp->tick(elapsed);
				updateSprite(sp,elapsed);				
			}
		}
		PR_END("SpritesObject::update")
	}

	// -------------------------------------------------------
	// Clear - set all sprites to not active
	// -------------------------------------------------------
	void clear() {
		for ( int i = 0; i < Size; ++i ) {
			Sprite* sp = &m_Sprites[i];
			sp->setActive(false);
		}
	}
	// -------------------------------------------------------
	// Virtual update sprite method
	// -------------------------------------------------------
	virtual void updateSprite(Sprite* sprite,float elapsed) = 0;

	void pushAway(Sprite* sprite,const Vector2f& pos,float minDistance,float push = 1.5f) {
		Vector2f diff = pos - sprite->getPosition();
		float dist = distance(pos,sprite->getPosition());
		float normDist = minDistance - dist;
		Vector2f nd = normalize(diff);
		nd *= -1.0f;
		if ( dist < minDistance ) {				
			normDist *= push;
			Vector2f hp = sprite->getPosition();
			hp += nd * normDist;
			sprite->setPosition(hp);
		}
	}

	void moveAndBounce(Sprite* sprite,float elapsed,const Rect& rect,bool calculateAngle = false) {
		Vector2f p = sprite->getPosition();
		Vector2f v = sprite->getVelocity();
		p += v * elapsed;
		if ( p.x < rect.left || p.x > rect.right ) {
			v.x *= -1.0f;
			sprite->setVelocity(v);
			p += v * elapsed;
			if ( calculateAngle ) {
				float angle = ds::math::getAngle(Vector2f(1,0),v);
				sprite->setAngle(angle);
			}
		}
		if ( p.y < rect.top || p.y > rect.bottom ) {
			v.y *= -1.0f;
			sprite->setVelocity(v);
			p += v * elapsed;
			if ( calculateAngle ) {
				float angle = ds::math::getAngle(Vector2f(1,0),v);
				sprite->setAngle(angle);
			}
		}
		sprite->setPosition(p);
	}
	void addCollider(Sprite* sprite,float radius) {
		assert(CollisionType != -1);
		sprite->setRadius(radius);
		m_CollisionManager->add(sprite,CollisionType);
	}
	void removeCollider(Sprite* sprite) {
		assert(CollisionType != -1);
		m_CollisionManager->remove(sprite,CollisionType);
	}

private:
	// -------------------------------------------------------
	// Find first non active sprite
	// -------------------------------------------------------
	int findFreeSprite() {
		for ( int i = 0; i < Size; ++i ) {
			ds::Sprite* sp = &m_Sprites[i];
			if ( !sp->isActive()) {
				return i;
			}
		}
		return -1;
	}
	Sprite m_Sprites[Size];
};

}