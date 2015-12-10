#include "SpriteTemplatesEditor.h"
#include "..\sprites\SpriteTemplates.h"
#include "..\DialogResources.h"

namespace ds {

	SpriteTemplatesEditor::SpriteTemplatesEditor(SpriteTemplates* templates) : AssetEditor(templates) , _templates(templates) {
		_offset = 0;
		_state = 1;
		_position = v2(20, 710);
	}


	SpriteTemplatesEditor::~SpriteTemplatesEditor() {
	}

	void SpriteTemplatesEditor::showDialog() {
		gui::start(SPRITE_TEMPLATES_ID, &_position);
		if (gui::begin("Sprite Templates", &_state)) {
			gui::ComboBox(SPRITE_TEMPLATES_ID + 20, &_model, &_offset);
			buttonGroup(SPRITE_TEMPLATES_ID + 9);
		}
		gui::end();

		if (_showAdd) {
			int ret = _dialog.show("Please provide a name", "Name");
			if (ret == 1) {
				int id = _templates->createEmptyTemplate(_dialog.getText());				
				_model.add(_dialog.getText(), id);
				_showAdd = false;
			}
			if (ret == 2) {
				_showAdd = false;
			}
		}

		if (_model.hasSelection()) {
			if (gui::begin("Sprite", &_state)) {
				int entry = _model.getSelectedValue();
				Sprite& sprite = _templates->get(entry);
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
			gui::end();
		}
	}

	bool SpriteTemplatesEditor::hasSelection() const {
		return _model.hasSelection();
	}

	const Sprite& SpriteTemplatesEditor::getSelection() const {
		int entry = _model.getSelectedValue();
		return _templates->get(entry);
	}

	Sprite& SpriteTemplatesEditor::getSelection() {
		int entry = _model.getSelectedValue();
		return _templates->get(entry);
	}

	void SpriteTemplatesEditor::init() {
		_model.clear();
		const TemplateMap& map = _templates->getTemplates();
		for (size_t i = 0; i < map.size(); ++i) {
			_model.add(map[i].name, map[i].sprite.id);
		}
	}
}