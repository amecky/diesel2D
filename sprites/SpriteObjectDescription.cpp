#include "SpriteObjectDescription.h"
#include "Sprite.h"
#include "..\compiler\Converter.h"

namespace ds {

void SpriteDescriptionManager::load(BinaryLoader* loader) {
	while ( loader->openChunk() == 0 ) {		
		if ( loader->getChunkID() == CHNK_SPRITE_DESCRIPTION ) {	
			int id = 0;
			loader->read(&id);
			SpriteDescription* description = &m_Descriptions[id];
			description->id = id;
			loader->read(&description->textureID);
			loader->read(&description->position);
			loader->read(&description->textureRect);
			loader->read(&description->angle);
			loader->read(&description->velocity);
			loader->read(&description->index);
			loader->read(&description->userValue);
			loader->read(&description->color);
			loader->read(&description->scale);
			loader->read(&description->radius);
		}
		loader->closeChunk();
	}		
}

}
