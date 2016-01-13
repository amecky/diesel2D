#pragma once
#include "..\io\DataFile.h"
#include "..\ui\IMGUI.h"

namespace ds {

	class AssetEditor {

	public:
		AssetEditor(DataFile* data) : _data(data) , _showAdd(false) {}
		virtual ~AssetEditor() {}

		virtual void showDialog() = 0;

		virtual void init() = 0;

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

	};

}