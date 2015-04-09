#pragma once
#include "Sprite.h"
#include "..\io\Serializer.h"

namespace ds {

	class SpriteTemplates : public Serializer {

	public:
		SpriteTemplates();
		~SpriteTemplates();
		void load(BinaryLoader* loader);
		const bool contains(int id) const;
		const Sprite& get(int id) const;
	private:
		Sprite m_Sprites[256];
		int m_Num;
		int m_IDs[256];
	};

}

