#pragma once
#include "..\base\GameState.h"
#include "SpriteTemplatesEditor.h"

namespace ds {

	class DialogEditorState : public GameState {

	public:
		DialogEditorState(DialogManager* manager);
		~DialogEditorState();
		int update(float dt);
		void render();
		int onGUIButton(DialogID dlgID, int button);
		void activate();
		void deactivate();
	private:
		void init();
		DialogManager* _manager;
		gui::ComponentModel<const char*> _model;
		v2 _dialogPos;
		int _dialogState;
		int _offset;
		gui::InputDialog _dialog;
		bool _showAdd;
	};

}

