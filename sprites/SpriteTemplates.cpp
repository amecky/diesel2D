#include "SpriteTemplates.h"
#include "..\compiler\Converter.h"

namespace ds {

	SpriteTemplates::SpriteTemplates() : m_Num(0) {
		for (int i = 0; i < 256; ++i) {
			m_IDs[i] = -1;
		}
	}


	SpriteTemplates::~SpriteTemplates() {
	}

	const bool SpriteTemplates::contains(int id) const {
		return m_IDs[id] != -1;
	}

	const Sprite& SpriteTemplates::get(int id) const {
		const int index = m_IDs[id];
		return m_Sprites[index];
	}

	void SpriteTemplates::load(BinaryLoader* loader) {
		while (loader->openChunk() == 0) {
			if (loader->getChunkID() == CHNK_SPRITE ) {
				int id = -1;
				loader->read(&id);
				if (id != -1) {
					Sprite& sp = m_Sprites[m_Num++];
					int tid = 0;
					loader->read(&tid);
					loader->read(&sp.position);
					Rect r;
					loader->read(&r);
					sp.texture = ds::math::buildTexture(r);
					sp.texture.textureID = tid;
					loader->read(&sp.scale);
					loader->read(&sp.rotation);
					loader->read(&sp.color);
					m_IDs[id] = m_Num - 1;
					LOGC("SpriteTemplates") << "added sprite: " << id << " at " << m_IDs[id];
				}
			}
			loader->closeChunk();
		}
	}
}