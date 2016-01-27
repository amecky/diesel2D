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
		if (_editor->showTexture()) {
			ds::sprites::draw(_editor->getTexturePosition(), math::buildTexture(0, 0, 1024, 1024), 0.0f, 1.0f, 1.0f);
		}
		_editor->showDialog();
		if (_editor->hasSelection()) {
			Sprite& sprite = _editor->getSelection();
			float cx = renderer::getScreenWidth() * 0.5f;
			float cy = renderer::getScreenHeight() * 0.5f;
			sprite.position = v2(cx, cy);
			sprites::draw(sprite);
			
		}
	}

	int SpriteTemplatesState::onButtonUp(int button, int x, int y) {
		if (_editor->showTexture()) {
			if ( button == 0) {
				_editor->setTexturePosition(x, y);
			}
			else {
				_editor->setDimension(x, y);
			}
		}
		return 0;
	}

	int SpriteTemplatesState::onChar(int ascii) {
		if (_editor->showTexture()) {
			if (ascii == 'a') {
				_editor->moveOffset(v2(-10, 0));
			}
			if (ascii == 'd') {
				_editor->moveOffset(v2(10, 0));
			}
			if (ascii == 'w') {
				_editor->moveOffset(v2(0, 10));
			}
			if (ascii == 's') {
				_editor->moveOffset(v2(0, -10));
			}
		}
		return 0;
	}

}