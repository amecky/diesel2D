#pragma once
#include "..\renderer\Renderer.h"
#include "..\sprites\Sprite.h"
#include "Actor.h"
#include "..\lib\DataArray.h"

namespace ds {
// -------------------------------------------------------
// SpriteRenderSystem
// -------------------------------------------------------
class SpriteRenderSystem {

typedef DataArray<sprite_t,MAX_ITEMS> SpriteList;
typedef std::vector<Animation*> Animations;
typedef std::map<IdString,uint32> AnimationMap;

public:
	SpriteRenderSystem(ds::Renderer* renderer);
	~SpriteRenderSystem(void);
	void render();
	void update(float elapsed);
	ID add(ID actorID,uint32 layer,const ds::Rect& textureRect,const Vector2f& scale,const ds::Color& color);
	sprite_t& lookup(ID id);
	void remove(ID id);
	void debug() {
		LOG << "num objects: " << m_SpriteList.numObjects;
		for ( uint32 i = 0; i < m_SpriteList.numObjects; ++i ) {
			LOG << i << " sprite " << m_SpriteList.objects[i].id << " actor " << m_SpriteList.objects[i].actorID << " animations " << m_SpriteList.objects[i].numAnimations;
		}
	}
	void setActorArray(DataArray<Actor,MAX_ITEMS>* actors) {
		m_Actors = actors;
	}
	void addAnimation(ID spriteID,Animation* animation);
	void addAnimation(ID spriteID,const char* name);
	void registerAnimation(const char* name,Animation* animation);
	template<class B>
	B* getAnimation(const char* name) {
		IdString hash = string::murmur_hash(name);
		if ( m_AnimationMap.find(hash) != m_AnimationMap.end()) {
			int idx = m_AnimationMap[hash];
			return static_cast<B*>(m_Animations[idx]);
		}
		return 0;
	}
private:
	uint32 find(Animation* animation) {
		for ( size_t i = 0; i < m_Animations.size(); ++i ) {
			if ( animation == m_Animations[i] ) {
				return i;
			}
		}
		return UINT_MAX;
	}
	ds::Renderer* m_Renderer;
	SpriteList m_SpriteList;
	DataArray<Actor,MAX_ITEMS>* m_Actors;
	Animations m_Animations;
	AnimationMap m_AnimationMap;
};

}