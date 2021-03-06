#include "AssetEditor.h"
#include "..\ui\IMGUI.h"
#include "..\renderer\graphics.h"
#include "..\data\DynamicSettingsEditor.h"

namespace ds {
	
	namespace editor {

		struct AssetEditorContext {

			bool active;
			int current;
			Array<AssetEditor*> editors;
			v2 position;

		};

		static AssetEditorContext* _editorContext = 0;

		void init() {
			_editorContext = new AssetEditorContext();
			_editorContext->active = false;
			_editorContext->position = v2(10, 700);
			_editorContext->current = -1;
		}

		void add(AssetEditor* editor) {
			_editorContext->editors.push_back(editor);
		}

		void addSettingsEditor(const char* shortcut, DynamicGameSettings* settings) {
			add(new DynamicSettingsEditor(shortcut, settings));
		}

		bool isActive() {
			return _editorContext->active;
		}

		bool toggle() {
			_editorContext->active = !_editorContext->active;
			if (_editorContext->active) {
				_editorContext->position.x = 10.0f;
				_editorContext->position.y = renderer::getScreenHeight() - 10.0f;
			}
			else {
				deactivateAll();
			}
			return _editorContext->active;
		}

		void deactivateAll() {
			for (int i = 0; i < _editorContext->editors.size(); ++i) {
				_editorContext->editors[i]->setActive(false);
			}
			_editorContext->current = -1;
		}

		void render() {
			if (_editorContext->active) {
				int _state = 1;
				gui::start(1, &_editorContext->position);
				if (gui::begin("Editors", &_state)) {
					gui::beginGroup();
					for (int i = 0; i < _editorContext->editors.size(); ++i) {
						if (gui::Button(_editorContext->editors[i]->getShortcut())) {
							_editorContext->editors[i]->setActive(!_editorContext->editors[i]->isActive());
							if (_editorContext->editors[i]->isActive()) {
								if (_editorContext->current != -1 && _editorContext->current != i) {
									_editorContext->editors[_editorContext->current]->setActive(false);
								}
								_editorContext->current = i;
								_editorContext->editors[i]->init();
							}
							else {
								_editorContext->current = -1;
							}
						}
					}
					gui::endGroup();
				}
				gui::end();
			}
			if (_editorContext->current != -1) {
				_editorContext->editors[_editorContext->current]->showDialog();
			}
		}

		void shutdown() {
			delete _editorContext;
		}
	}

}