#pragma once
#include "Sprite.h"
#include "..\io\Serializer.h"
#include <map>

namespace ds {

	class SpriteTemplates : public Serializer {

	typedef std::map<IdString, Sprite> TemplateMap;
	
	public:
		SpriteTemplates();
		~SpriteTemplates();
		void load(BinaryLoader* loader);
		const bool contains(int id) const;
		//const Sprite& get(int id) const;
		bool get(const char* name,Sprite* sprite);
	private:
		TemplateMap m_Map;
		Sprite m_Sprites[256];
		int m_Num;
		int m_IDs[256];
	};

}

