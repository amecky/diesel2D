#pragma once
#include "..\base\GameObject.h"
#include "..\utils\Profiler.h"

namespace ds {

// -------------------------------------------------------
// SpritesObject - container of sprites
// -------------------------------------------------------
template<int Size>
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