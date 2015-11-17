#pragma once
#include "..\compiler\DataFile.h"
#include "..\ui\IMGUI.h"

namespace ds {

	class AssetEditor {

	public:
		AssetEditor(DataFile* data) : _data(data) , _showAdd(false) {}
		virtual ~AssetEditor() {}

		virtual void showDialog() = 0;

		virtual void init() = 0;

	protected:
		void buttonGroup(int startIndex) {
			gui::beginGroup();
			if (gui::Button(startIndex, "Add")) {
				_showAdd = true;
			}
			if (gui::Button(startIndex + 1, "Load")) {
				_data->load();
				init();
			}
			if (gui::Button(startIndex + 2, "Save")) {
				_data->save();
			}
			if (gui::Button(startIndex + 3, "Import")) {
				_data->importJSON();
				init();
			}
			if (gui::Button(startIndex + 4, "Export")) {
				_data->exportJSON();
			}
			gui::endGroup();
		}
		bool _showAdd;
		DataFile* _data;

	};

}