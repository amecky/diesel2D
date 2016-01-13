#pragma once
#include "Sprite.h"
#include "..\lib\collection_types.h"
#include "..\ui\IMGUI.h"
#include "..\io\DataFile.h"

namespace ds {

struct MappingEntry {
	IdString hash;
	Sprite sprite;
	char name[32];
};

typedef Array<MappingEntry> TemplateMap;

class SpriteTemplates : public DataFile {

public:
	SpriteTemplates();
	~SpriteTemplates();
	const bool contains(int id) const;
	bool get(const char* name,Sprite* sprite);
	Sprite& get(int index);
	const Sprite& get(int index) const;
	bool saveData(JSONWriter& writer);
	const char* getFileName() const {
		return "resources\\sprite_templates.json";
	}
	int createEmptyTemplate(const char* name);
	const TemplateMap& getTemplates() const {
		return _map;
	}
	virtual bool loadData(JSONReader& loader);
private:
	const int getIndex(int id) const;
	const int getIndex(IdString hash) const;
	TemplateMap _map;
	int _currentID;
};

}

