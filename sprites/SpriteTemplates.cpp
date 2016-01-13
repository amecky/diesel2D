#include "SpriteTemplates.h"
#include "..\DialogResources.h"
#include "..\io\json.h"
#include "..\utils\Profiler.h"

namespace ds {

	SpriteTemplates::SpriteTemplates() : DataFile(){
		_currentID = 0;
	}


	SpriteTemplates::~SpriteTemplates() {
	}

	const int SpriteTemplates::getIndex(int id) const {
		for (size_t i = 0; i < _map.size(); ++i) {
			if (_map[i].sprite.id == id) {
				return i;
			}
		}
		return -1;
	}

	const int SpriteTemplates::getIndex(IdString hash) const {
		for (size_t i = 0; i < _map.size(); ++i) {
			if (_map[i].hash == hash) {
				return i;
			}
		}
		return -1;
	}

	int SpriteTemplates::createEmptyTemplate(const char* name) {
		MappingEntry entry;
		strcpy(entry.name, name);
		entry.hash = string::murmur_hash(entry.name);
		entry.sprite.id = _currentID++;
		entry.sprite.position = v2(512, 384);
		entry.sprite.rotation = 0.0f;
		entry.sprite.scale = v2(1, 1);
		entry.sprite.texture = math::buildTexture(0, 0, 100, 100);
		entry.sprite.type = 0;
		entry.sprite.color = Color::WHITE;
		_map.push_back(entry);
		return entry.sprite.id;
	}

	const bool SpriteTemplates::contains(int id) const {
		return getIndex(id) != -1;
	}

	Sprite& SpriteTemplates::get(int index) {
		return _map[index].sprite;
	}

	const Sprite& SpriteTemplates::get(int index) const {
		return _map[index].sprite;
	}

	bool SpriteTemplates::get(const char* name, Sprite* sprite) {
		IdString hash = string::murmur_hash(name);
		int idx = getIndex(hash);
		if (idx != -1) {
			const Sprite& sp = _map[idx].sprite;
			sprite->id = sp.id;
			sprite->position = sp.position;
			sprite->rotation = sp.rotation;
			sprite->scale = sp.scale;
			sprite->texture = sp.texture;
			sprite->type = sp.type;
			sprite->color = sp.color;
			return true;
		}
		else {
			sprite->id = 0;
			sprite->position = v2(512,384);
			sprite->rotation = 0.0f;
			sprite->scale = v2(1, 1);
			sprite->texture = math::buildTexture(0,0,100,100);
			sprite->type = 0;
			sprite->color = Color::WHITE;
		}
		return true;

	}
	/*
	bool SpriteTemplates::saveData(JSONWriter& writer) {
		for (size_t i = 0; i < _map.size(); ++i) {
			const MappingEntry& entry = _map[i];
			writer.startCategory("sprite");
			writer.write("name", entry.name);
			writer.write("texture_id", entry.sprite.texture.textureID);
			writer.write("position", entry.sprite.position);
			writer.write("rect", entry.sprite.texture.rect);
			writer.write("scale", entry.sprite.scale);
			writer.write("rotation", entry.sprite.rotation);
			writer.write("color", entry.sprite.color);
			writer.write("type", entry.sprite.type);
			writer.endCategory();
		}
		return true;
	}
	*/
	bool SpriteTemplates::saveData(JSONWriter& writer) {
		for (size_t i = 0; i < _map.size(); ++i) {
			const MappingEntry& entry = _map[i];
			writer.startCategory("sprite");
			writer.write("name", entry.name);
			writer.write("texture_id", entry.sprite.texture.textureID);
			writer.write("position", entry.sprite.position);
			writer.write("rect", entry.sprite.texture.rect);
			writer.write("scale", entry.sprite.scale);
			writer.write("rotation", entry.sprite.rotation);
			writer.write("color", entry.sprite.color);
			writer.write("type", entry.sprite.type);
			writer.endCategory();
		}
		return true;
	}

	bool SpriteTemplates::loadData(const JSONReader& loader) {
		int cats[256];
		int num = loader.get_categories(cats, 256);
		for (int i = 0; i < num; ++i) {
			MappingEntry entry;
			Rect rect;
			const char* name = loader.get_string(cats[i], "name");
			sprintf_s(entry.name, 32, "%s", name);
			//LOG << "name: " << name;
			entry.hash = string::murmur_hash(entry.name);
			loader.get_int(cats[i], "texture_id", &entry.sprite.texture.textureID);
			loader.get_vec2(cats[i], "position", &entry.sprite.position);
			loader.get_rect(cats[i], "rect", &rect);
			entry.sprite.texture = math::buildTexture(rect);
			loader.get_vec2(cats[i], "scale", &entry.sprite.scale);
			loader.get_float(cats[i], "rotation", &entry.sprite.rotation);
			loader.get_color(cats[i], "color", &entry.sprite.color);
			loader.get_int(cats[i], "type", &entry.sprite.type);
			entry.sprite.texture = math::buildTexture(rect);
			entry.sprite.id = _currentID++;
			_map.push_back(entry);

		}
		return true;
	}
	
}