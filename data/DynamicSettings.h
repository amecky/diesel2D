#pragma once
#include "..\math\vector.h"
#include "..\math\math_types.h"
#include "..\renderer\render_types.h"
#include "..\utils\Color.h"
#include "..\lib\collection_types.h"
#include "..\ui\IMGUI.h"

namespace ds {

	enum SettingsType {
		ST_FLOAT,
		ST_RECT,
		ST_INT,
		ST_V2_PATH,
		ST_COLOR,
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
	struct DynamicGameSettings : public DataFile {

		DynamicGameSettings();
		~DynamicGameSettings();

		void addFloat(const char* name, float* value, float defaultValue);
		bool setFloat(const char* name, float value);

		void addInt(const char* name, int* value, int defaultValue);
		bool setInt(const char* name, int value);

		void addRect(const char* name, Rect* value, const Rect& defaultValue);
		bool setRect(const char* name, const Rect& value);

		void addColor(const char* name, Color* value, const Color& defaultValue);
		bool setColor(const char* name, const Color& value);

		void addPath(const char* name, Vector2fPath* value);
		bool setPath(const char* name, const Vector2fPath& value);

		//void load();

		//void save();

		void showDialog(v2* pos);

		void showCompleteDialog(v2* pos);

		//virtual const char* getName() const = 0;

		bool saveData(JSONWriter& writer);
		bool loadData(const JSONReader& loader);
		virtual const char* getFileName() const = 0;

		int _state;
		int _offset;
		Array<SettingsItem> _items;
		Array<float*> _floats;
		Array<int*> _ints;
		Array<Rect*> _rects;
		Array<Color*> _colors;
		Array<Vector2fPath*> _v2_paths;
		gui::ComponentModel<SettingsItem> _model;
	};
}