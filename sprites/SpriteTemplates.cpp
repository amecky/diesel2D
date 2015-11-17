#include "SpriteTemplates.h"
#include "..\compiler\Converter.h"
#include "..\DialogResources.h"
#include "..\utils\JSONWriter.h"

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
	
	bool SpriteTemplates::loadData(BinaryLoader& loader) {
		_map.clear();
		_currentID = 0;
		while (loader.openChunk() == 0) {
			if (loader.getChunkID() == 1) {
				MappingEntry entry;
				Rect r;
				std::string name;
				loader.read(name);
				strcpy(entry.name, name.c_str());
				entry.hash = string::murmur_hash(entry.name);
				loader.read(&entry.sprite.texture.textureID);
				loader.read(&entry.sprite.position);
				loader.read(&r);
				entry.sprite.texture = math::buildTexture(r);
				loader.read(&entry.sprite.scale);
				loader.read(&entry.sprite.rotation);
				loader.read(&entry.sprite.color);
				loader.read(&entry.sprite.type);
				entry.sprite.id = _currentID++;
				_map.push_back(entry);
			}
			loader.closeChunk();
		}
		return true;
	}

	bool SpriteTemplates::saveData(BinaryWriter& writer) {
		for (size_t i = 0; i < _map.size(); ++i) {
			const MappingEntry& entry = _map[i];
			writer.startChunk(1, 1);
			writer.write(entry.name);
			writer.write(entry.sprite.texture.textureID);
			writer.write(entry.sprite.position);
			writer.write(entry.sprite.texture.rect);
			writer.write(entry.sprite.scale);
			writer.write(entry.sprite.rotation);
			writer.write(entry.sprite.color);
			writer.write(entry.sprite.type);
			writer.closeChunk();
		}
		return true;
	}

	bool SpriteTemplates::exportData(JSONWriter& writer) {
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

	bool SpriteTemplates::importData(JSONReader& reader) {
		_map.clear();
		_currentID = 0;
		std::vector<Category*> categories = reader.getCategories();
		for (size_t i = 0; i < categories.size(); ++i) {
			Category* c = categories[i];
			LOG << "name: " << c->getName();
			if (c->getName() == "sprite") {
				MappingEntry entry;
				Rect r;
				std::string name = c->getProperty("name");
				strcpy(entry.name, name.c_str());
				entry.hash = string::murmur_hash(entry.name);
				c->getInt("texture_id", &entry.sprite.texture.textureID);
				c->getVector2f("position", &entry.sprite.position);
				c->getRect("rect", &r);
				entry.sprite.texture = math::buildTexture(r);
				c->getVector2f("scale", &entry.sprite.scale);
				c->getFloat("rotation", &entry.sprite.rotation);
				c->getColor("color", &entry.sprite.color);
				c->getInt("type", &entry.sprite.type);
				entry.sprite.id = _currentID++;
				_map.push_back(entry);
			}
		}
		return true;
	}
	
}