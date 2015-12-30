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
		ds::sprites::draw(v2(512, 384), math::buildTexture(0, 0, 1024, 1024),0.0f,1.0f,1.0f);
		_editor->showDialog();
		if (_editor->hasSelection()) {
			Sprite& sprite = _editor->getSelection();
			float cx = renderer::getScreenWidth() * 0.5f;
			float cy = renderer::getScreenHeight() * 0.5f;
			sprite.position = v2(cx, cy);
			sprites::draw(sprite);
		}
	}

}