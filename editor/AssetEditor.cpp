#include "AssetEditor.h"
#include "..\ui\IMGUI.h"

namespace ds {

	AssetEditorManager::AssetEditorManager() : _active(false) , _position(10,700) {

	}
	AssetEditorManager::~AssetEditorManager() {

	}
	
	void AssetEditorManager::add(AssetEditor* editor) {
		_editors.push_back(editor);
	}
	
	void AssetEditorManager::activate(const char* name) {

	}
	
	void AssetEditorManager::deactivate(const char* name) {

	}

	void AssetEditorManager::deactivateAll() {

	}

	void AssetEditorManager::toggle() {
		_active = !_active;
	}

	void AssetEditorManager::render() {
		// render buttons
		if (_active) {
			int _state = 1;
			gui::start(1, &_position);
			if (gui::begin("Editors", &_state)) {
				gui::beginGroup();
				for (int i = 0; i < _editors.size(); ++i) {
					if (gui::Button(_editors[i]->getShortcut())) {
						_editors[i]->setActive(!_editors[i]->isActive());
						if (_editors[i]->isActive()) {
							_editors[i]->init();
						}
					}
				}
				gui::endGroup();
			}
			gui::end();
		}
		for (int i = 0; i < _editors.size(); ++i) {
			if (_editors[i]->isActive()) {
				_editors[i]->showDialog();
			}
		}
	}

}