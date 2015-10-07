#include "BitmapFont.h"
#include "..\compiler\Converter.h"

namespace ds {

	void BitmapFont::intialize(const FontDefinition& definition) {
		startChar = definition.startChar;
		endChar = definition.endChar;
		width = definition.width;
		height = definition.height;
		padding = definition.padding;
		textureSize = definition.textureSize;
		charHeight = definition.charHeight;
		startX = definition.startX;
		startY = definition.startY;
		gridHeight = definition.gridHeight;
	}

	void BitmapFont::load(BinaryLoader* loader) {
		//clear();
		while ( loader->openChunk() == 0 ) {
			if ( loader->getChunkID() == CHNK_FONT ) {	
				loader->read(&startChar);
				loader->read(&endChar);
				loader->read(&charHeight);
				loader->read(&gridHeight);
				loader->read(&startX);
				loader->read(&startY);
				loader->read(&width);
				loader->read(&height);
				loader->read(&padding);
				loader->read(&textureSize);			
				loader->closeChunk();	
			}
		}
	}

}
