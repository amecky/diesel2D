#include "PathConverter.h"

namespace ds {

	void PathConverter::convert(JSONReader& reader, BinaryWriter& writer) {
		Category* root = reader.getCategory("path");
		if ( root != 0 ) {
			std::vector<Category*> categories = root->getChildren();
			for ( size_t i = 0; i < categories.size(); ++i ) {
				Category* c = categories[i];
				if ( c->getName() == "start" ) {
					Vector2f p0 = c->getVector2f("p0");
					Vector2f p1 = c->getVector2f("p1");
					Vector2f p2 = c->getVector2f("p2");
					Vector2f p3 = c->getVector2f("p3");
					writer.startChunk(1,1);
					writer.write(p0);
					writer.write(p1);
					writer.write(p2);
					writer.write(p3);
					writer.closeChunk();				
				}
				else if ( c->getName() == "element" ) {
					Vector2f p1 = c->getVector2f("p1");
					Vector2f p2 = c->getVector2f("p2");
					Vector2f p3 = c->getVector2f("p3");
					writer.startChunk(2,1);
					writer.write(p1);
					writer.write(p2);
					writer.write(p3);
					writer.closeChunk();				
				}
			}
		}
		writer.close();		
	}

}