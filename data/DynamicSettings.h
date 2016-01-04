#pragma once
#include "..\math\vector.h"
#include "..\math\math_types.h"
#include "..\renderer\render_types.h"
#include "..\utils\Color.h"
#include <vector>
#include "..\ui\IMGUI.h"

namespace ds {

	enum SettingsType {
		ST_FLOAT,
		ST_RECT,
		ST_INT,
		ST_NONE
	};

	// -------------------------------------------------------
	// settings item
	// -------------------------------------------------------
	struct SettingsItem {
		const char* name;
		SettingsType type;
		int index;
	};

	// -------------------------------------------------------
	// dynamic settings
	// -------------------------------------------------------
	struct DynamicGameSettings {

		DynamicGameSettings();
		~DynamicGameSettings();

		void addFloat(const char* name, float* value, float defaultValue);
		bool setFloat(const char* name, float value);

		void addInt(const char* name, int* value, int defaultValue);
		bool setInt(const char* name, int value);

		void addRect(const char* name, Rect* value, const Rect& defaultValue);
		bool setRect(const char* name, const Rect& value);

		void load();

		void save();

		void showDialog(v2* pos);

		void showCompleteDialog(v2* pos);

		virtual const char* getName() const = 0;

		int _state;
		int _offset;
		std::vector<SettingsItem> _items;
		std::vector<float*> _floats;
		std::vector<int*> _ints;
		std::vector<Rect*> _rects;
		gui::ComponentModel<SettingsItem> _model;
	};
}