#pragma once
#include "..\math\vector.h"
#include "..\math\math_types.h"
#include "..\utils\Color.h"
#include <vector>
#include "..\ui\IMGUI.h"

namespace ds {

	// -------------------------------------------------------
	// settings item
	// -------------------------------------------------------
	struct SettingsItem {
		const char* name;
		int type;
		int index;
	};

	// -------------------------------------------------------
	// dynamic settings
	// -------------------------------------------------------
	struct DynamicGameSettings {

		DynamicGameSettings();
		~DynamicGameSettings();

		void addFloat(const char* name, float* value, float defaultValue);

		void exportJSON();

		void importJSON();

		void load();

		void save();

		void showDialog();

		int _state;
		int _offset;
		std::vector<SettingsItem> _items;
		std::vector<float*> _floats;
		gui::ComponentModel<SettingsItem> _model;
	};
}