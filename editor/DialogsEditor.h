#pragma once
#include "..\dialogs\DialogManager.h"
#include "AssetEditor.h"

namespace ds {

	class DialogsEditor : public AssetEditor {

	public:
		DialogsEditor(DialogManager* manager);
		~DialogsEditor();

		void showDialog();

		void init();

	private:
		DialogManager* _manager;
		gui::ComponentModel<const char*> _model;
		v2 _dialogPos;
		int _dialogState;
		int _offset;
		gui::InputDialog _dialog;
	};

}