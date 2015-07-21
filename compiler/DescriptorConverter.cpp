#include "DescriptorConverter.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

	DescriptorConverter::DescriptorConverter() : Converter("DescriptorConverter") {}

	DescriptorConverter::~DescriptorConverter(void) {}

	/*
	"name" : "GridDesc" ,
	"texture" : "Grid" ,
	"shader" : "TexturedColor" ,
	"blend_state" : "default"
	*/
	void DescriptorConverter::convert(JSONReader& reader, BinaryWriter& writer) {
		std::vector<Category*> categories = reader.getCategories();
		IdString tmp;
		for (size_t i = 0; i < categories.size(); ++i) {
			Category* c = categories[i];
			if (c->getName() == "descriptor") {
				writer.startChunk(CHNK_DESCRIPTOR, 1);
				c->getIdString("name", &tmp);
				writer.write(tmp);
				c->getIdString("texture", &tmp);
				if (c->getProperty("texture") == "none") {
					tmp = 0;
				}
				writer.write(tmp);
				c->getIdString("shader", &tmp);
				writer.write(tmp);
				c->getIdString("blend_state", &tmp);
				writer.write(tmp);
				writer.closeChunk();
			}
		}
	}

}
