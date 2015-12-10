#pragma once
#include "..\base\GameState.h"
#include "SpriteTemplatesEditor.h"

namespace ds {

	class SpriteTemplatesState : public GameState {

	public:
		SpriteTemplatesState();
		~SpriteTemplatesState();
		int update(float dt);
		void render();
		int onGUIButton(DialogID dlgID, int button);
		void activate();
		void deactivate();
	private:
		SpriteTemplatesEditor* _editor;
	};

}

