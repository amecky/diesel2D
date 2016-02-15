#pragma once
#include "..\io\DataFile.h"
#include "..\ui\IMGUI.h"

namespace ds {

	class AssetEditor {

	public:
		AssetEditor(const char* name, DataFile* data) : _name(name), _data(data), _showAdd(false) , _active(false) {}
		virtual ~AssetEditor() {}

		virtual void showDialog() = 0;

		virtual void init() = 0;

		bool isActive() const {
			return _active;
		}
		void setActive(bool active) {
			_active = active;
		}
		virtual const char* getShortcut() const = 0;

	protected:
		void buttonGroup() {
			gui::beginGroup();
			if (gui::Button("Add")) {
				_showAdd = true;
			}
			if (gui::Button("Load")) {
				_data->load();
				init();
			}
			if (gui::Button("Save")) {
				_data->save();
			}
			gui::endGroup();
		}
		bool _showAdd;
		DataFile* _data;
	private:
		const char* _name;
		bool _active;
	};

	class AssetEditorManager {

	public:
		AssetEditorManager();
		~AssetEditorManager();
		void add(AssetEditor* editor);
		void addDefaultEditors();
		void deactivateAll();
		void render();
		void toggle();
	private:
		bool _active;
		int _current;
		Array<AssetEditor*> _editors;
		v2 _position;
	};

}