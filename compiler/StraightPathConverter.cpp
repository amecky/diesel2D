#include "StraightPathConverter.h"

namespace ds {

	void StraightPathConverter::convert(JSONReader& reader, BinaryWriter& writer) {
		Category* root = reader.getCategory("path");
		if ( root != 0 ) {
			std::vector<Category*> categories = root->getChildren();
			for ( size_t i = 0; i < categories.size(); ++i ) {
				Category* c = categories[i];
				if ( c->getName() == "start" ) {
					Vector2f p0 = c->getVector2f("start");
					Vector2f p1 = c->getVector2f("end");
					writer.startChunk(1,1);
					writer.write(p0);
					writer.write(p1);
					writer.closeChunk();				
				}
				else if ( c->getName() == "element" ) {
					Vector2f p1 = c->getVector2f("end");
					writer.startChunk(2,1);
					writer.write(p1);
					writer.closeChunk();				
				}
			}
		}
		writer.close();		
	}

	void StraightPath3DConverter::convert(JSONReader& reader, BinaryWriter& writer) {
		Category* root = reader.getCategory("path");
		if (root != 0) {
			std::vector<Category*> categories = root->getChildren();
			for (size_t i = 0; i < categories.size(); ++i) {
				Category* c = categories[i];
				if (c->getName() == "start") {
					Vector3f p0 = c->getVector3f("start");
					Vector3f p1 = c->getVector3f("end");
					writer.startChunk(1, 1);
					writer.write(p0);
					writer.write(p1);
					writer.closeChunk();
				}
				else if (c->getName() == "element") {
					Vector3f p1 = c->getVector3f("end");
					writer.startChunk(2, 1);
					writer.write(p1);
					writer.closeChunk();
				}
			}
		}
		writer.close();
	}

}