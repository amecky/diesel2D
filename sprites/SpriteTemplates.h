#pragma once
#include "Sprite.h"
#include "..\io\Serializer.h"
#include <map>
#include "..\ui\IMGUI.h"

namespace ds {

class SpriteTemplates {

struct MappingEntry {
	IdString hash;
	Sprite sprite;
	char name[32];
};

typedef std::vector<MappingEntry> TemplateMap;

public:
	SpriteTemplates();
	~SpriteTemplates();
	void load();
	void save();
	void exportJSON();
	void importJSON();
	const bool contains(int id) const;
	bool get(const char* name,Sprite* sprite);
	void showDialog();
private:
	const int getIndex(int id) const;
	const int getIndex(IdString hash) const;
	TemplateMap _map;
	int _state;
	int _currentID;
	gui::ComponentModel<MappingEntry> _model;
	int _offset;
	bool _showAdd;
	gui::InputDialog _dialog;
};

}

