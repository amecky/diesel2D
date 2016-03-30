#include "SpriteTemplatesEditor.h"
#include "..\sprites\SpriteTemplates.h"
#include "..\DialogResources.h"
#include "..\renderer\graphics.h"

namespace ds {

	SpriteTemplatesEditor::SpriteTemplatesEditor(SpriteTemplates* templates) : AssetEditor("SPT", templates) , _templates(templates) {
		_offset = 0;
		_state = 1;
		_position = v2(20, 710);
	}


	SpriteTemplatesEditor::~SpriteTemplatesEditor() {
	}

	void SpriteTemplatesEditor::showDialog() {
		gui::start(SPRITE_TEMPLATES_ID, &_position);
		if (gui::begin("Sprite Templates", &_state)) {
			gui::DropDownBox(&_model, &_offset);
			buttonGroup();
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
				gui::InputInt("Texture_id", &sprite.texture.textureID);
				gui::InputVec2("Position", &sprite.position);
				gui::InputRect("TextureRect", &r);
				gui::InputVec2("Scale", &sprite.scale);
				gui::InputFloat("Rotation", &sprite.rotation);
				gui::InputColor("Color", &sprite.color);
				gui::InputInt("Type", &sprite.type);
				gui::Image("Sprite", r, sprite.texture.textureID, sprite.color);
				sprite.texture = math::buildTexture(r);
			}
			gui::end();
		}
	}

	void SpriteTemplatesEditor::init() {
		_model.clear();
		const TemplateMap& map = _templates->getTemplates();
		for (size_t i = 0; i < map.size(); ++i) {
			_model.add(map[i].name, map[i].sprite.id);
		}
	}
}