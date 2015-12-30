#pragma once
#include "Sprite.h"
#include "..\io\Serializer.h"
#include <map>
#include "..\ui\IMGUI.h"
#include "..\compiler\DataFile.h"

namespace ds {

struct MappingEntry {
	IdString hash;
	Sprite sprite;
	char name[32];
};

typedef std::vector<MappingEntry> TemplateMap;

class SpriteTemplates : public DataFile {

public:
	SpriteTemplates();
	~SpriteTemplates();
	const bool contains(int id) const;
	bool get(const char* name,Sprite* sprite);
	Sprite& get(int index);
	const Sprite& get(int index) const;
	bool saveData(JSONWriter& writer);
	bool loadData(JSONReader& reader);
	const char* getFileName() const {
		return "resources\\sprite_templates.json";
	}
	int createEmptyTemplate(const char* name);
	const TemplateMap& getTemplates() const {
		return _map;
	}
private:
	const int getIndex(int id) const;
	const int getIndex(IdString hash) const;
	TemplateMap _map;
	int _currentID;
};

}

