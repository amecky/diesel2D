#pragma once
#include "..\base\GameObject.h"
#include "..\utils\Log.h"
#include "..\utils\Profiler.h"

namespace ds {

template<int Size>
class CompositeSprite : public GameObject {

public:

	CompositeSprite() : GameObject() {
		m_NumObjects = 0;
		m_FreeList = 0;
		m_ManageLifecycle = false;
	}

	virtual ~CompositeSprite() {}

	void add(const Vector2f& pos,const Rect& r,float rotation = 0.0f,const Vector2f& size = Vector2f(1,1),const Color& color = Color::WHITE) {
		if ( m_NumObjects < Size - 1 ) {
			Sprite &o = m_Sprites[m_FreeList];
			o.setActive(true);
			o.setPosition(pos);
			o.setTextureRect(r);
			o.setAngle(rotation);
			o.setScale(size);
			o.setColor(color);
			o.resetTimer();
			++m_FreeList;
			++m_NumObjects;
		}
		else {
			LOGE << "No more sprites available";
		}
	}
	
	void update( float elapsed ) {
		PR_START("CompositeSprite::update")
		uint16 idx = 0;
		while ( idx < m_NumObjects ) {
			m_Sprites[idx].tick(elapsed);
			if ( !updateSprite(&m_Sprites[idx]) ) {
				removeByIndex(idx);
			}
			else {
				++idx;
			}
		}
		if ( m_ManageLifecycle ) {
			manageLifecycle();
		}
		PR_END("CompositeSprite::update")
	}

	void removeByIndex(int idx) {
		if ( m_NumObjects > 0 ) {
			m_Sprites[idx] = m_Sprites[m_NumObjects-1];
			--m_NumObjects;
			--m_FreeList;
		}
		else {
			m_FreeList = 0;
			m_NumObjects = 0;
		}
	}

	virtual bool updateSprite(Sprite* sprite) {
		return true;
	}

	void render() {
		for ( uint32 i = 0; i < m_NumObjects; ++i ) {
			m_Renderer->draw(m_Sprites[i]);
		}
	}
	const uint16 num() const {
		return m_NumObjects;
	}
	void manageLifecycle() {
		uint16 idx = 0;
		while ( idx < m_NumObjects ) {
			if ( m_Sprites[idx].getTimer() >= m_TTL ) {
				removeByIndex(idx);
			}
			else {
				++idx;
			}
		}
		
	}
	void setTTL(float ttl) {
		m_ManageLifecycle = true;
		m_TTL = ttl;
	}
	void clear() {
		m_NumObjects = 0;
		m_FreeList = 0;
	}
private:
	bool m_ManageLifecycle;
	float m_TTL;
	uint16 m_FreeList;
	uint16 m_NumObjects;
	Sprite m_Sprites[Size];
};

}

