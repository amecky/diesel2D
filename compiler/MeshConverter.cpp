#include "MeshConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"
#include "..\renderer\graphics.h"

namespace ds {

	MeshConverter::MeshConverter() : Converter("MeshConverter") {}

	MeshConverter::~MeshConverter(void) {}

	/*
	"name" : "Block" ,
	"data" : "block" ,
	"vertex_type" : "PNC" ,
	"descriptor" : "GridDesc"
	*/
	void MeshConverter::convert(JSONReader& reader, BinaryWriter& writer) {
		std::vector<Category*> categories = reader.getCategories();
		IdString tmp;
		for (size_t i = 0; i < categories.size(); ++i) {
			Category* c = categories[i];
			if (c->getName() == "mesh") {
				writer.startChunk(CHNK_MESH, 1);
				c->getIdString("name", &tmp);
				writer.write(tmp);
				c->getIdString("data", &tmp);
				writer.write(tmp);
				int vt = renderer::getVDType(c->getProperty("vertex_type").c_str());
				writer.write(vt);
				c->getIdString("descriptor", &tmp);
				writer.write(tmp);
				writer.closeChunk();
			}
		}
	}

}
