#pragma once
#include "Sprite.h"
#include "..\io\Serializer.h"
#include <map>
#include "..\ui\IMGUI.h"

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
	void showDialog(v2* pos);
private:
	TemplateMap _map;
	//Sprite _sprites[256];
	//int _num;
	int _IDs[256];
	int _state;
	//int _selected;
	gui::ComponentModel<IdString> _model;
	int _offset;
};

}

