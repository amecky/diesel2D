#pragma once
#include "..\utils\StringUtils.h"

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
};

class Converter {

public:
	Converter(const char* name) {
		m_HashName = string::murmur_hash(name);
	}
	virtual ~Converter() {}
	virtual void convert(const char* fileName) = 0;
	const IdString& getHashName() const {
		return m_HashName;
	}
private:
	IdString m_HashName;
};

}