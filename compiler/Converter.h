#pragma once
#include "..\utils\StringUtils.h"
#include "..\utils\PlainTextReader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

enum ChunkType {
	CHNK_GUI = 0,
	CHNK_DLG_DEF,
	CHNK_DLG_IMAGE,
	CHNK_DLG_BUTTON,
	CHNK_DLG_TEXT,
	CHNK_FONT,
	CHNK_PARTICLE_DATA,
	CHNK_RING_EMITTER,
	CHNK_EMITTER_DATA,
	CHNK_PARTICLE_SIZE,
	CHNK_PARTICLE_COLOR,
	CHNK_PARTICLE_VELOCITY,
	CHNK_PARTICLE_ROTATION,
	CHNK_SPRITE,
	CHNK_PARTICLESYSTEM,
	CHNK_BLOOM_COMPONENT,
	CHNK_BOX_EMITTER,
	CHNK_SPRITE_DESCRIPTION,
	CHNK_DLG_IMAGE_LINK
};

class Converter {

public:
	Converter(const char* name) {
		m_HashName = string::murmur_hash(name);
	}
	virtual ~Converter() {}

	virtual void convert(const char* fileName) {
		char buffer[256];
		sprintf(buffer,"%s\\%s.json",getResourceDirectory(),fileName);
		LOG << "loading file: " << buffer;
		char out[256];
		IdString dataName = string::murmur_hash(buffer);
		sprintf(out,"data\\%u",dataName);
		LOG << "writing to: " << out;
		BinaryWriter writer;
		int signature[] = {0,8,15};
		writer.open(out,signature,3);
		JSONReader reader;
		if ( reader.parse(buffer) ) {
			convert(reader,writer);
			writer.close();
		}
		else {
			LOGEC("Converter") << "Cannot parse " << fileName;
		}
	}

	const IdString& getHashName() const {
		return m_HashName;
	}

	virtual void convert(JSONReader& reader,BinaryWriter& writer) = 0;
	virtual const char* getResourceDirectory()  = 0;
	virtual const char* getEnding() const {
		return "json";
	}
private:
	IdString m_HashName;
};

}