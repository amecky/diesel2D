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
	bool get(const char* name,Sprite* sprite);
	void showDialog();
private:
	TemplateMap _map;
	int _IDs[256];
	int _state;
	gui::ComponentModel<IdString> _model;
	int _offset;
};

}

