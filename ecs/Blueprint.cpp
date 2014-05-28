#include "Blueprint.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\Log.h"
#include <vector>
#include "World.h"
#include "Animations.h"

namespace ds {

Blueprint::Blueprint() : m_SpriteDefinition(0) , m_ColliderDefinition(0) {
	m_ActorTranslator.add("position",&ActorDefinition::position);
	m_ActorTranslator.add("rotation",&ActorDefinition::angle);
	m_ActorTranslator.add("type",&ActorDefinition::type);
	// sprite definition translator
	m_SpriteTranslator.add("layer",&SpriteDefinition::layer);
	m_SpriteTranslator.add("texture_rect",&SpriteDefinition::textureRect);
	m_SpriteTranslator.add("angle",&SpriteDefinition::angle);
	m_SpriteTranslator.add("scale",&SpriteDefinition::scale);
	m_SpriteTranslator.add("color",&SpriteDefinition::color);
	// collider
	m_ColliderTranslator.add("radius",&ColliderDefinition::radius);
}


Blueprint::~Blueprint() {
	delete m_ColliderDefinition;
	delete m_SpriteDefinition;
}

// -------------------------------------------------------
// load json file
// -------------------------------------------------------
void Blueprint::load(const char* name,World* world) {
	char buffer[256];
	sprintf(buffer,"content\\blueprints\\%s.json",name);
	JSONReader reader;
	if ( reader.parse(buffer) ) {
		Category* root = reader.getCategory("blueprint");
		if ( root != 0 ) {
			m_ActorTranslator.read(root,&m_ActorDefinition);
			std::vector<Category*> categories= root->getChildren();
			for ( size_t i = 0; i < categories.size(); ++i ) {
				Category* c = categories[i];
				if ( c->getName() == "sprite" ) {
					m_SpriteDefinition = new SpriteDefinition;
					m_SpriteTranslator.read(c,m_SpriteDefinition);
					LOG << "sprite rect " << DBG_RECT(m_SpriteDefinition->textureRect);
				}
				if ( c->getName() == "collider" ) {
					m_ColliderDefinition = new ColliderDefinition;
					m_ColliderTranslator.read(c,m_ColliderDefinition);
				}
				else if ( c->getName() == "animations" ) {
					LOG << "found animations";
					std::vector<Category*> animations = c->getChildren();
					for ( size_t j = 0; j < animations.size(); ++j ) {
						Category* ac = animations[j];						
						Animation* anim = createAnimation(ac->getName());
						if ( anim != 0 ) {
							LOG << "animation: '" << ac->getName() << "'";
							DynamicSettings& settings = anim->getSettings();
							settings.read(ac);
							world->registerAnimation(ac->getProperty("name").c_str(),anim);
							m_AnimationNames.push_back(ac->getProperty("name"));
						}
					}
				}
				else if ( c->getName() == "behaviors" ) {
					std::vector<Category*> behaviors = c->getChildren();
					for ( size_t j = 0; j < behaviors.size(); ++j ) {
						Category* ac = behaviors[j];						
						Behavior* behavior = createBehavior(ac->getName());
						if ( behavior != 0 ) {
							LOG << "behavior: '" << ac->getName() << "'";
							m_BehaviorNames.push_back(ac->getProperty("name"));
							DynamicSettings& settings = behavior->getSettings();
							settings.read(ac);
							world->registerBehavior(ac->getProperty("name").c_str(),behavior);
						}						
					}
				}
			}
		}
	}
	else {
		LOGE << "Cannot find " << buffer;
	}
}

// -------------------------------------------------------
// create behavior by name
// -------------------------------------------------------
Behavior* Blueprint::createBehavior(const std::string& name) {
	if ( name == "FixedLifetimeBehavior" ) {
		return new FixedLifetimeBehavior;
	}
	else if ( name == "BulletBehavior" ) {
		return new BulletBehavior;		
	}
	else if ( name == "TargetMoveBehavior" ) {
		return new TargetMoveBehavior;		
	}
	else if ( name == "GravityBehavior" ) {
		return new GravityBehavior;
	}
	else if ( name == "SimpleMoveBehavior" ) {
		return new SimpleMoveBehavior;
	}
	else if ( name == "BounceMoveBehavior" ) {
		return new BounceMoveBehavior;
	}
	return 0;
}

// -------------------------------------------------------
// create animation by name
// -------------------------------------------------------
Animation* Blueprint::createAnimation(const std::string& name) {
	if ( name == "ColorFadeAnimation" ) {
		return new ColorFadeAnimation;
	}
	else if ( name == "SizeAnimation" ) {
		return new SizeAnimation;
	}
	else if ( name == "GrowSizeAnimation" ) {
		return new GrowSizeAnimation;
	}
	else if ( name == "StaticRotationAnimation" ) {
		return new StaticRotationAnimation;
	}
	return 0;
}

// -------------------------------------------------------
// Create new actor
// -------------------------------------------------------
ID Blueprint::initialize(World* world) {
	ID id = world->create(m_ActorDefinition.type,m_ActorDefinition.position,m_ActorDefinition.angle);
	if ( m_SpriteDefinition != 0 ) {
		world->attachSprite(id,m_SpriteDefinition->layer,m_SpriteDefinition->textureRect,m_SpriteDefinition->angle,m_SpriteDefinition->scale,m_SpriteDefinition->color);
	}
	if ( m_ColliderDefinition != 0 ) {
		world->attachCollider(id,m_ColliderDefinition->radius);
	}
	for ( size_t i = 0; i < m_BehaviorNames.size(); ++i ) {
		std::string name = m_BehaviorNames[i];
		world->addBehavior(id,name.c_str());
	}
	for ( size_t i = 0; i < m_AnimationNames.size(); ++i ) {
		std::string name = m_AnimationNames[i];
		world->addAnimation(id,name.c_str());
	}
	return id;
}

}