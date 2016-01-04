#include "SpriteGroup.h"
#include "SpriteBatch.h"
#include "..\ui\IMGUI.h"
#include "..\DialogResources.h"

namespace ds {

	SpriteGroup::SpriteGroup(const char* name) {
		_name = string::murmur_hash(name);
		_position = v2(512, 384);
		_state = 1;
		_offset = 0;
		_currentSID = 0;
	}


	SpriteGroup::~SpriteGroup() {
	}

	void SpriteGroup::render() {
		for (size_t i = 0; i < _sprites.size(); ++i) {
			sprites::draw(_sprites[i]);
		}
	}

	void SpriteGroup::load() {
		char buffer[128];
		sprintf_s(buffer, 128, "assets\\%u", _name);
		LOG << "loading binary file: " << buffer;
		FILE* f = fopen(buffer, "rb");
		_currentSID = 0;
		if (f) {
			int nr = 0;
			fread(&nr, sizeof(int), 1, f);
			for (int i = 0; i < nr; ++i) {
				Sprite sp;
				fread(&sp, sizeof(Sprite), 1, f);
				sp.id = _currentSID;
				_sprites.push_back(sp);
				char buffer[32];
				sprintf_s(buffer, 32, "Sprite %d", sp.id);
				_model.add(buffer, sp.id);
				++_currentSID;
			}
			fclose(f);
		}
	}

	void SpriteGroup::save() {
		char buffer[128];
		sprintf_s(buffer, 128, "assets\\%u", _name);
		LOG << "saving binary file: " << buffer;
		FILE* f = fopen(buffer, "wb");
		if (f) {
			int nr = _sprites.size();
			fwrite(&nr, sizeof(int), 1, f);
			for (size_t i = 0; i < _sprites.size(); ++i) {
				fwrite(&_sprites[i], sizeof(Sprite), 1, f);
			}
			fclose(f);
		}
	}

	void SpriteGroup::showDialog() {
		gui::start(SPRITE_GROUP_ID, &_position);
		if (gui::begin("SpriteGroup", &_state)) {
			gui::ComboBox(&_model, &_offset);
			gui::beginGroup();
			if (gui::Button("Add")) {
				Sprite sp;
				sp.id = _currentSID;
				sp.position = v2(512, 384);
				sp.texture = math::buildTexture(0, 0, 50, 50);
				_sprites.push_back(sp);
				char buffer[32];
				sprintf_s(buffer, 32, "Sprite %d", _currentSID);
				_model.add(buffer, _currentSID);
				++_currentSID;
			}
			if (gui::Button("Save")) {
				save();
			}
			if (gui::Button("Up")) {
			}
			if (gui::Button("Down")) {
			}
			gui::endGroup();
		}
		gui::end();
		if (_model.hasSelection()) {
			if (gui::begin("Sprite", &_state)) {
				int idx = _model.getSelection();
				Sprite& sprite = _sprites[idx];
				Rect r = sprite.texture.rect;
				gui::InputInt("Texture_id", &sprite.texture.textureID);
				gui::InputVec2("Position", &sprite.position);
				gui::InputRect("TextureRect", &r);
				gui::InputVec2("Scale", &sprite.scale);
				gui::InputFloat("Rotation", &sprite.rotation);
				gui::InputColor("Color", &sprite.color);
				gui::InputInt("Type", &sprite.type);
				sprite.texture = math::buildTexture(r);

			}
			gui::end();
		}

	}

	SpriteGroupContainer::~SpriteGroupContainer() {
		GroupMapping::iterator it = _map.begin();
		while (it != _map.end()) {
			delete it->second;
			it = _map.erase(it);
		}
	}

	bool SpriteGroupContainer::contains(const char* name) const {
		return _map.find(name) != _map.end();
	}

	SpriteGroup* SpriteGroupContainer::get(const char* name) {
		if (_map.find(name) != _map.end()) {
			return _map[name];
		}
		return 0;
	}

	SpriteGroup* SpriteGroupContainer::create(const char* name) {
		SpriteGroup* group = new SpriteGroup(name);
		_map[name] = group;
		_model.add(name, group);
		return group;
	}

	void SpriteGroupContainer::save() {
		/*
		BinaryWriter writer;
		char buffer[64];
		IdString hash = string::murmur_hash("SpriteGroupContainer");
		sprintf_s(buffer, 64, "data\\%u", hash);
		int signature[] = { 0, 8, 15 };
		if (writer.open(buffer, signature, 3) == 0) {



			writer.close();
		}
		*/
	}

	void SpriteGroupContainer::showDialog() {
		gui::start(SPRITE_GROUP_CONTAINER_ID, &_position);
		if (gui::begin("SpriteGroupContainer", &_state)) {
			gui::ComboBox(&_model, &_offset);
		}
		gui::end();
		if (_model.hasSelection()) {
			SpriteGroup* group = _model.getSelectedValue();
			group->showDialog();
		}
	}

}