#include "SpriteTemplates.h"
#include "..\compiler\Converter.h"
#include "..\DialogResources.h"
#include "..\utils\JSONWriter.h"

namespace ds {

	SpriteTemplates::SpriteTemplates() : _offset(0) , _state(1) {
		_currentID = 0;
		_showAdd = false;
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

	const bool SpriteTemplates::contains(int id) const {
		return getIndex(id) != -1;
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
	const Sprite& SpriteTemplates::get(int id) const {
		const int index = m_IDs[id];
		return m_Sprites[index];
	}
	*/
	void SpriteTemplates::load() {
		_map.clear();
		_model.clear();
		BinaryLoader loader;
		char buffer[64];
		sprintf(buffer, "assets\\%u", string::murmur_hash("sprite_templates"));
		LOGC("SpriteTemplates") << "loading file: " << buffer;
		int signature[] = { 0, 8, 15 };
		loader.open(buffer, signature, 3);
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
				_map.push_back(entry);
				_model.add(name.c_str(), entry);
			}
			loader.closeChunk();
		}
	}

	void SpriteTemplates::save() {
		char buffer[64];
		sprintf(buffer, "assets\\%u", string::murmur_hash("sprite_templates"));
		LOGC("SpriteTemplates") << "saving file: " << buffer;
		BinaryWriter writer;
		int signature[] = { 0, 8, 15 };
		writer.open(buffer, signature, 3);
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
		writer.close();
	}

	void SpriteTemplates::exportJSON() {
		char buffer[64];
		sprintf(buffer, "content\\resources\\sprite_templates.json");
		JSONWriter jw;
		jw.open(buffer);
		for (size_t i = 0; i < _map.size(); ++i) {
			const MappingEntry& entry = _map[i];
			jw.startCategory("sprite");
			jw.write("name",entry.name);
			jw.write("texture_id", entry.sprite.texture.textureID);
			jw.write("position", entry.sprite.position);
			jw.write("rect", entry.sprite.texture.rect);
			jw.write("scale", entry.sprite.scale);
			jw.write("rotation", entry.sprite.rotation);
			jw.write("color", entry.sprite.color);
			jw.write("type", entry.sprite.type);
			jw.endCategory();
		}
	}

	void SpriteTemplates::importJSON() {
		_model.clear();
		_map.clear();
		char buffer[64];
		sprintf(buffer, "content\\resources\\sprite_templates.json");
		JSONReader reader;
		if (reader.parse(buffer)) {
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
					//c->getProperty("name", entry.name);
					c->getInt("texture_id", &entry.sprite.texture.textureID);
					c->getVector2f("position", &entry.sprite.position);
					c->getRect("rect", &r);
					entry.sprite.texture = math::buildTexture(r);
					c->getVector2f("scale", &entry.sprite.scale);
					c->getFloat("rotation", &entry.sprite.rotation);
					c->getColor("color", &entry.sprite.color);
					c->getInt("type", &entry.sprite.type);
					_map.push_back(entry);
					_model.add(name.c_str(), entry);
				}
			}
		}
		else {
			LOG << "Error: Cannot parse file: " << buffer;
		}
	}
	
	void SpriteTemplates::showDialog() {
		if (gui::begin("Sprite Templates", &_state)) {
			gui::ComboBox(SPRITE_TEMPLATES_ID + 20, &_model, &_offset);
			gui::beginGroup();
			if (gui::Button(SPRITE_TEMPLATES_ID + 9, "Add")) {
				_showAdd = true;
			}
			if (gui::Button(SPRITE_TEMPLATES_ID + 13, "Load")) {
				load();
			}
			if (gui::Button(SPRITE_TEMPLATES_ID + 10, "Save")) {
				save();
			}
			if (gui::Button(SPRITE_TEMPLATES_ID + 11, "Import")) {
				importJSON();
			}
			if (gui::Button(SPRITE_TEMPLATES_ID + 12, "Export")) {
				exportJSON();
			}
			gui::endGroup();
		}
		gui::end();

		if (_showAdd) {
			int ret = _dialog.show("Please provide a name", "Name");
			if (ret == 1) {
				MappingEntry entry;				
				strcpy(entry.name, _dialog.getText());
				entry.hash = string::murmur_hash(entry.name);
				entry.sprite.id = _currentID++;
				entry.sprite.position = v2(512, 384);
				entry.sprite.rotation = 0.0f;
				entry.sprite.scale = v2(1, 1);
				entry.sprite.texture = math::buildTexture(0, 0, 100, 100);
				entry.sprite.type = 0;
				entry.sprite.color = Color::WHITE;
				_map.push_back(entry);
				_model.add(entry.name, entry);
				_showAdd = false;
			}
			if (ret == 2) {
				_showAdd = false;
			}
		}

		if (_model.hasSelection()) {
			if (gui::begin("Sprite", &_state)) {					
				MappingEntry entry = _model.getSelectedValue();
				int idx = getIndex(entry.hash);
				if ( idx != -1) {
					Sprite& sprite = _map[idx].sprite;
					ds::Rect r = sprite.texture.rect;
					gui::InputInt(SPRITE_TEMPLATES_ID + 2, "Texture_id", &sprite.texture.textureID);
					gui::InputVec2(SPRITE_TEMPLATES_ID + 3, "Position", &sprite.position);
					gui::InputRect(SPRITE_TEMPLATES_ID + 4, "TextureRect", &r);
					gui::InputVec2(SPRITE_TEMPLATES_ID + 5, "Scale", &sprite.scale);
					gui::InputFloat(SPRITE_TEMPLATES_ID + 6, "Rotation", &sprite.rotation);
					gui::InputColor(SPRITE_TEMPLATES_ID + 7, "Color", &sprite.color);
					gui::InputInt(SPRITE_TEMPLATES_ID + 8, "Type", &sprite.type);
					sprite.texture = math::buildTexture(r);
				}
			}
			gui::end();
		}
		
	}
}