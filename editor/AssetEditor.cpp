#include "AssetEditor.h"
#include "..\ui\IMGUI.h"
#include "..\renderer\graphics.h"

namespace ds {

	AssetEditorManager::AssetEditorManager() : _active(false) , _position(10,700) , _current(-1) {
		
	}
	AssetEditorManager::~AssetEditorManager() {

	}
	
	void AssetEditorManager::add(AssetEditor* editor) {
		_editors.push_back(editor);
	}
	
	void AssetEditorManager::toggle() {
		_active = !_active;
		if (_active) {
			_position.x = 10.0f;
			_position.y = renderer::getScreenHeight() - 10.0f;
		}
		else {
			deactivateAll();
		}
	}

	void AssetEditorManager::deactivateAll() {
		for (int i = 0; i < _editors.size(); ++i) {
			_editors[i]->setActive(false);
		}
		_current = -1;
	}

	void AssetEditorManager::render() {
		if (_active) {
			int _state = 1;
			gui::start(1, &_position);
			if (gui::begin("Editors", &_state)) {
				gui::beginGroup();
				for (int i = 0; i < _editors.size(); ++i) {
					if (gui::Button(_editors[i]->getShortcut())) {
						_editors[i]->setActive(!_editors[i]->isActive());
						if (_editors[i]->isActive()) {
							if (_current != -1 && _current != i) {
								_editors[_current]->setActive(false);
							}
							_current = i;
							_editors[i]->init();
						}
						else {
							_current = -1;
						}
					}
				}
				gui::endGroup();
			}
			gui::end();
		}
		if (_current != -1) {
			_editors[_current]->showDialog();
		}
	}

}