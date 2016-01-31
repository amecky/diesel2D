#include "SpriteTemplatesEditor.h"
#include "..\sprites\SpriteTemplates.h"
#include "..\DialogResources.h"
#include "..\renderer\graphics.h"

namespace ds {

	SpriteTemplatesEditor::SpriteTemplatesEditor(SpriteTemplates* templates) : AssetEditor("SPT", templates) , _templates(templates) {
		_offset = 0;
		_state = 1;
		_position = v2(20, 710);
		_showTexture = false;
	}


	SpriteTemplatesEditor::~SpriteTemplatesEditor() {
	}

	void SpriteTemplatesEditor::showDialog() {
		gui::start(SPRITE_TEMPLATES_ID, &_position);
		if (gui::begin("Sprite Templates", &_state)) {
			gui::ComboBox(&_model, &_offset);
			buttonGroup();
			if (_showTexture) {
				if (gui::Button("Hide Texture")) {
					_showTexture = false;
				}
			}
			else {
				if (gui::Button("Show Texture")) {
					_showTexture = true;
				}
			}
			if (_showTexture) {
				char buffer[128];
				sprintf_s(buffer, 128, "Top: %g Left: %g", _texturePos.y, _texturePos.x);
				gui::Label(buffer);
				sprintf_s(buffer, 128, "W: %g H: %g", _dim.x, _dim.y);
				gui::Label(buffer);
				if (_model.hasSelection()) {
					if (gui::Button("Copy texture rect")) {
						// set rect
						int entry = _model.getSelectedValue();
						Sprite& sprite = _templates->get(entry);
						sprite.texture = math::buildTexture(_texturePos.y, _texturePos.x, _dim.x, _dim.y);
					}
				}
			}

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
		_textureOffset = v2(-350, 0);
		_texturePos = v2(512, 512);
		_dim = v2(0, 0);
	}

	v2 SpriteTemplatesEditor::getTexturePosition() const {
		float cx = renderer::getScreenWidth() * 0.5f - _textureOffset.x;
		float cy = renderer::getScreenHeight() * 0.5f - _textureOffset.y;
		return v2(cx, cy);
	}

	void SpriteTemplatesEditor::setTexturePosition(int x, int y) {
		if (x > 460) {
			v2 p = v2(x, renderer::getScreenHeight() - y);
			float dx = (renderer::getScreenWidth() - 1024.0f) / 2.0f;
			float dy = (renderer::getScreenHeight() - 1024.0f) / 2.0f;
			_texturePos.x = x - dx + _textureOffset.x;
			_texturePos.y = y - dy - _textureOffset.y;
			_dim = v2(0, 0);
		}
	}

	void SpriteTemplatesEditor::setDimension(int x,int y) {
		if (x > 460) {
			v2 p = v2(x, renderer::getScreenHeight() - y);
			float dx = (renderer::getScreenWidth() - 1024.0f) / 2.0f;
			float dy = (renderer::getScreenHeight() - 1024.0f) / 2.0f;
			float w = x - dx + _textureOffset.x;
			float h = y - dy - _textureOffset.y;
			_dim.x = w - _texturePos.x;
			_dim.y = h - _texturePos.y;
		}
	}
}