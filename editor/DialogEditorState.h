#pragma once
#include "..\base\GameState.h"
#include "SpriteTemplatesEditor.h"

namespace ds {

	class DialogEditorState : public AssetEditor {

	public:
		DialogEditorState(DialogManager* manager);
		~DialogEditorState();
		//int update(float dt);
		void showDialog();
		//int onGUIButton(DialogID dlgID, int button);
		//void activate();
		//void deactivate();
		void init();
		const char* getShortcut() const {
			return "DLG";
		}
	private:		
		DialogManager* _manager;
		gui::ComponentModel<const char*> _model;
		v2 _dialogPos;
		int _dialogState;
		int _offset;
		gui::InputDialog _dialog;
		bool _showAdd;
	};

}

