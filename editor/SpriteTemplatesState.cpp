#include "SpriteTemplatesState.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\base\GameStateMachine.h"

namespace ds {

	SpriteTemplatesState::SpriteTemplatesState() : GameState("SpriteTemplatesState"), _editor(0) {
	}


	SpriteTemplatesState::~SpriteTemplatesState() {
	}

	// --------------------------------------------
	// activate
	// --------------------------------------------
	void SpriteTemplatesState::activate() {
		if (_editor == 0) {
			_editor = new ds::SpriteTemplatesEditor(renderer::getSpriteTemplates());
			_editor->init();
		}
		_showSprite = false;
		_offset = v2(0, 0);
		_texturePos = v2(512, 512);
		_dim = v2(0, 0);
		LOG << "Press '1' to show or hide entire texture atlas";
		LOG << "Press 'a', 'w', 's' , 'd' to scroll texture atlas";
		LOG << "Click to get texture location";
	}

	// --------------------------------------------
	// activate
	// --------------------------------------------
	void SpriteTemplatesState::deactivate() {
		if (_editor != 0) {
			delete _editor;
			_editor = 0;
		}
	}

	// --------------------------------------------
	// update
	// --------------------------------------------
	int SpriteTemplatesState::update(float dt) {
		return 0;
	}

	// --------------------------------------------
	// click
	// --------------------------------------------
	int SpriteTemplatesState::onGUIButton(ds::DialogID dlgID, int button) {
		return button;
	}

	// --------------------------------------------
	// render
	// --------------------------------------------
	void SpriteTemplatesState::render() {
		if (_showSprite) {
			float cx = renderer::getScreenWidth() * 0.5f - _offset.x;
			float cy = renderer::getScreenHeight() * 0.5f - _offset.y;
			ds::sprites::draw(v2(cx, cy), math::buildTexture(0, 0, 1024, 1024), 0.0f, 1.0f, 1.0f);
		}
		_editor->showDialog();
		if (_showSprite) {
			int s = 1;
			v2 p = v2(800, 720);
			gui::start(1, &p);
			if (gui::begin("Sprite Templates", &s)) {
				char buffer[128];
				sprintf_s(buffer, 128, "Top: %g Left: %g", _texturePos.y, _texturePos.x);
				gui::Label(buffer);
				sprintf_s(buffer, 128, "W: %g H: %g", _dim.x, _dim.y);
				gui::Label(buffer);
			}
			gui::end();
		}
		if (_editor->hasSelection()) {
			Sprite& sprite = _editor->getSelection();
			float cx = renderer::getScreenWidth() * 0.5f;
			float cy = renderer::getScreenHeight() * 0.5f;
			sprite.position = v2(cx, cy);
			sprites::draw(sprite);
			
		}
	}

	int SpriteTemplatesState::onButtonUp(int button, int x, int y) {
		if (_showSprite) {				
			v2 p = v2(x,renderer::getScreenHeight() - y);
			float dx = (renderer::getScreenWidth() - 1024.0f) / 2.0f;
			float dy = (renderer::getScreenHeight() - 1024.0f) / 2.0f;
			if (button == 0) {
				_texturePos.x = x - dx + _offset.x;
				_texturePos.y = y - dy - _offset.y;
				_dim = v2(0, 0);
			}
			else {
				float w = x - dx + _offset.x;
				float h = y - dy - _offset.y;
				_dim.x = w - _texturePos.x;
				_dim.y = h - _texturePos.y;
			}
		}
		return 0;
	}

	int SpriteTemplatesState::onChar(int ascii) {
		if (ascii == '1') {
			_showSprite = !_showSprite;
			if (!_showSprite) {
				_offset = v2(0, 0);
			}
		}
		if (ascii == 'a' && _showSprite) {
			_offset.x -= 10;
		}
		if (ascii == 'd' && _showSprite) {
			_offset.x += 10;
		}
		if (ascii == 'w' && _showSprite) {
			_offset.y += 10;
		}
		if (ascii == 's' && _showSprite) {
			_offset.y -= 10;
		}		
		return 0;
	}

}