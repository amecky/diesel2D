#include "DialogEditorState.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\base\GameStateMachine.h"

namespace ds {

	DialogEditorState::DialogEditorState(DialogManager* manager) : AssetEditor("DialogEditorState",0) , _manager(manager) {
	}


	DialogEditorState::~DialogEditorState() {
	}

	void DialogEditorState::init() {
		_dialogPos = v2(10, 710);
		_dialogState = 1;
		_offset = 0;
		_showAdd = false;
		const Dialogs& dialogs = _manager->getDialogs();
		_model.clear();
		for (size_t i = 0; i < dialogs.size(); ++i) {
			_model.add(dialogs[i].name, dialogs[i].name);
		}
	}

	// --------------------------------------------
	// render
	// --------------------------------------------
	void DialogEditorState::showDialog() {
		gui::start(280, &_dialogPos);
		if (gui::begin("Dialogs", &_dialogState)) {
			gui::ComboBox(&_model, &_offset);
			gui::beginGroup();
			if (gui::Button("Activate")) {
				if (_model.hasSelection()) {
					const char* name = _model.getSelectedValue();
					_manager->activate(name);
				}
			}
			else if (gui::Button("Deactivate")) {
				if (_model.hasSelection()) {
					const char* name = _model.getSelectedValue();
					_manager->deactivate(name);
				}
			}
			gui::endGroup();
			gui::beginGroup();
			if (gui::Button("Add")) {
				_showAdd = true;
			}
			if (gui::Button("Load")) {
				const char* name = _model.getSelectedValue();
				GUIDialog* dlg = _manager->get(name);
				if (dlg != 0) {
					dlg->load();
				}
				init();
			}
			if (gui::Button("Save")) {
				const char* name = _model.getSelectedValue();
				GUIDialog* dlg = _manager->get(name);
				if (dlg != 0) {
					dlg->save();
				}
			}
			gui::endGroup();
			gui::beginGroup();
			if (gui::Button("Remove")) {
				if (_model.hasSelection()) {
					const char* name = _model.getSelectedValue();
					if (_manager->remove(name)) {
						_model.remove(_model.getSelection());
						_model.select(-1);
					}
				}
			}
			gui::endGroup();
		}
		gui::end();
		if (_showAdd) {
			int ret = _dialog.show("Please provide a name", "Name");
			if (ret == 1) {
				DialogDefinition def;
				strcpy(def.name, _dialog.getText());
				if (_manager->contains(def.name) == 0) {
					GUIDialog* dialog = _manager->create(def.name);
					_model.add(def.name, dialog->getName());
				}
				_showAdd = false;
			}
			if (ret == 2) {
				_showAdd = false;
			}
		}
		if (_model.hasSelection()) {
			const char* name = _model.getSelectedValue();
			GUIDialog* dlg = _manager->get(name);
			if (dlg != 0) {
				dlg->showDialog();
			}
		}
	}

}