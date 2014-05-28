#include "SpriteRenderSystem.h"

namespace ds {

SpriteRenderSystem::SpriteRenderSystem(ds::Renderer* renderer) : m_Renderer(renderer) {
}

sprite_t& SpriteRenderSystem::lookup(ID id) {
	return m_SpriteList.get(id);
}

SpriteRenderSystem::~SpriteRenderSystem(void) {
	Animations::iterator it = m_Animations.begin();
	while ( it != m_Animations.end() ) {
		delete (*it);
		it = m_Animations.erase(it);
	}
}

ID SpriteRenderSystem::add(ID actorID,uint32 layer,const ds::Rect& textureRect,const Vector2f& scale,const ds::Color& color) {
	ID id = m_SpriteList.add();
	sprite_t& s = m_SpriteList.get(id);
	s.textureRect = textureRect;
	s.color = color;
	s.layer = layer;
	s.scale = scale;
	s.actorID = actorID;
	s.numAnimations = 0;
	return id;
}

void SpriteRenderSystem::remove(ID id) {
	m_SpriteList.remove(id);
}

// -------------------------------------------------------
// add animation
// -------------------------------------------------------
void SpriteRenderSystem::addAnimation(ID spriteID,Animation* animation) {
	uint32 index = find(animation);
	if ( index == UINT_MAX ) {
		index = m_Animations.size();
		m_Animations.push_back(animation);
	}
	sprite_t& entry = m_SpriteList.get(spriteID);
	entry.animations[entry.numAnimations++] = index;
}

// -------------------------------------------------------
// add animation by name
// -------------------------------------------------------
void SpriteRenderSystem::addAnimation(ID spriteID,const char* name) {
	IdString hash = string::murmur_hash(name);
	assert(m_AnimationMap.find(hash) != m_AnimationMap.end());
	sprite_t& entry = m_SpriteList.get(spriteID);
	entry.animations[entry.numAnimations++] = m_AnimationMap[hash];
}

// -------------------------------------------------------
// Render
// -------------------------------------------------------
void SpriteRenderSystem::render() {
	for ( uint32 l = 0; l < 16; ++l ) {
		for ( uint32 i = 0; i < m_SpriteList.numObjects; ++i ) {
			sprite_t& s = m_SpriteList.objects[i];
			if ( s.layer == l ) {
				Actor& actor = m_Actors->get(s.actorID);
				m_Renderer->draw(actor.position,0,s.textureRect,actor.angle,s.scale.x,s.scale.y,s.color);
			}
		}
	}
}

// -------------------------------------------------------
// Update
// -------------------------------------------------------
void SpriteRenderSystem::update(float elapsed) {
	for ( uint32 i = 0; i < m_SpriteList.numObjects; ++i ) {
		sprite_t& s = m_SpriteList.objects[i];
		Actor& a = m_Actors->get(s.actorID);
		if ( s.numAnimations > 0 ) {
			for ( uint32 j = 0; j < s.numAnimations; ++j ) {
				Animation* anim = m_Animations[s.animations[j]];
				anim->update(a,s,elapsed);
			}
		}
	}
}

// -------------------------------------------------------
// Register behavior by name
// -------------------------------------------------------
void SpriteRenderSystem::registerAnimation(const char* name,Animation* animation) {
	IdString hash = string::murmur_hash(name);
	if ( m_AnimationMap.find(hash) == m_AnimationMap.end()) {
		uint32 idx = find(animation);
		if ( idx == UINT_MAX ) {
			idx = m_Animations.size();
			m_Animations.push_back(animation);
		}
		m_AnimationMap[hash] = idx;
	}
	else {
		uint32 idx = m_AnimationMap[hash];
		assert(m_Animations[idx] == animation);
	}
}


}


