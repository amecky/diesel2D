#pragma once
#include "Sprite.h"
#include "..\io\Serializer.h"
#include <map>

namespace ds {

	class SpriteTemplates : public Serializer {

	typedef std::map<IdString, Sprite> TemplateMap;
	typedef std::vector<std::string> Names;

	public:
		SpriteTemplates();
		~SpriteTemplates();
		void load(BinaryLoader* loader);
		const bool contains(int id) const;
		//const Sprite& get(int id) const;
		bool get(const char* name,Sprite* sprite);
		void showDialog(v2* pos,const char* name);
	private:
		Names _names;
		TemplateMap _map;
		Sprite _sprites[256];
		int _num;
		int _IDs[256];
		int _state;
	};

}

