#include "BlueprintConverter.h"
#include "..\io\BinaryWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\ecs\World.h"

namespace ds {


BlueprintConverter::BlueprintConverter() : Converter("BlueprintConverter") {
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

void BlueprintConverter::convert(JSONReader& reader,BinaryWriter& writer) {
	Category* root = reader.getCategory("blueprint");
	if ( root != 0 ) {
		ActorDefinition actorDefinition;
		m_ActorTranslator.read(root,&actorDefinition);
		m_ActorTranslator.saveChunk(writer,1000,&actorDefinition);
		std::vector<Category*> categories= root->getChildren();
		for ( size_t i = 0; i < categories.size(); ++i ) {
			Category* c = categories[i];
			if ( c->getName() == "collider" ) {
				ColliderDefinition colliderDefinition;
				m_ColliderTranslator.read(c,&colliderDefinition);
				colliderDefinition.used = true;
				m_ColliderTranslator.saveChunk(writer,1001,&colliderDefinition);
			}
			else if ( c->getName() == "behaviors" ) {
				std::vector<Category*> behaviors = c->getChildren();
				for ( size_t j = 0; j < behaviors.size(); ++j ) {
					Category* ac = behaviors[j];						
					Behavior* behavior = m_World->findBehavior(ac->getName().c_str());
					if ( behavior != 0 ) {
						writer.startChunk(1002,1);
						writer.write(ac->getName());
						behavior->convert(ac,writer);
						writer.closeChunk();
					}	
				}
			}
		}		
	}
}

}