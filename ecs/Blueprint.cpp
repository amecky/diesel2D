#include "Blueprint.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\Log.h"
#include <vector>
#include "World.h"
#include "Animations.h"

namespace ds {

Blueprint::Blueprint(World* world) : m_World(world) {
	m_ActorTranslator.add("position",&ActorDefinition::position);
	m_ActorTranslator.add("angle",&ActorDefinition::angle);
	m_ActorTranslator.add("type",&ActorDefinition::type);
	m_ActorTranslator.add("layer",&ActorDefinition::layer);
	m_ActorTranslator.add("texture_rect",&ActorDefinition::textureRect);
	m_ActorTranslator.add("color",&ActorDefinition::color);
	m_ActorTranslator.add("scale",&ActorDefinition::scale);
	// collider
	m_ColliderTranslator.add("radius",&ColliderDefinition::radius);
}


Blueprint::~Blueprint() {}

// -------------------------------------------------------
// load file
// -------------------------------------------------------
void Blueprint::load( BinaryLoader* loader ) {
	LOG << "loading blueprint";
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == 1000 ) {		
			m_ActorTranslator.readChunk(*loader,&m_ActorDefinition);
		}
		else if ( loader->getChunkID() == 1001 ) {					
			m_ColliderTranslator.readChunk(*loader,&m_ColliderDefinition);
			m_ColliderDefinition.used = true;
		}
		else if ( loader->getChunkID() == 1002 ) {
			std::string name;
			loader->read(name);
			LOG << "name " << name;
			BehaviorDefinition def;
			def.index = m_World->findBehaviorIndex(name.c_str());			
			Behavior* behavior = m_World->findBehavior(name.c_str());
			def.definitionID  = behavior->createDefinition(loader);
			m_BehaviorDefinitions.push_back(def);
		}
		loader->closeChunk();
	}		
}

// -------------------------------------------------------
// Create new actor
// -------------------------------------------------------
ID Blueprint::initialize(World* world) {
	ID id = world->create(m_ActorDefinition.layer,m_ActorDefinition.type,m_ActorDefinition.position,m_ActorDefinition.textureRect,m_ActorDefinition.angle,m_ActorDefinition.scale,m_ActorDefinition.color);
	if ( m_ColliderDefinition.used ) {
		world->attachCollider(id,m_ColliderDefinition.radius);
	}
	Actor& actor = m_World->getActor(id);
	for ( size_t i = 0; i < m_BehaviorDefinitions.size(); ++i ) {
		BehaviorDefinition& def = m_BehaviorDefinitions[i];
		Behavior* behavior = world->addBehavior(id,def.index);
		behavior->init(actor,def.definitionID);

	}
	return id;
}

}