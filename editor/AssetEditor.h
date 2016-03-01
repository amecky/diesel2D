#pragma once
#include "..\io\DataFile.h"
#include "..\ui\IMGUI.h"
#include "..\data\DynamicSettings.h"

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

	namespace editor {

		void init();

		void add(AssetEditor* editor);

		void addSettingsEditor(const char* shortcut,DynamicGameSettings* settings);

		void deactivateAll();

		void render();

		bool toggle();

		bool isActive();

		void shutdown();

	}

}