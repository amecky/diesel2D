#pragma once
#include "DynamicSettings.h"
#include "..\DialogResources.h"

namespace ds {

	DynamicGameSettings::DynamicGameSettings() : _state(1), _offset(0) {}

	DynamicGameSettings::~DynamicGameSettings() {}

	// -------------------------------------------------------
	// add int
	// -------------------------------------------------------
	void DynamicGameSettings::addInt(const char* name, int* value, int defaultValue) {
		*value = defaultValue;
		SettingsItem item;
		item.name = name;
		item.type = ST_INT;
		item.index = _ints.size();
		_ints.push_back(value);
		_items.push_back(item);
		_model.add(name, item);
	}

	// -------------------------------------------------------
	// set float
	// -------------------------------------------------------
	bool DynamicGameSettings::setInt(const char* name, int value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*_ints[item.index] = value;
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// add float
	// -------------------------------------------------------
	void DynamicGameSettings::addFloat(const char* name, float* value, float defaultValue) {
		*value = defaultValue;
		SettingsItem item;
		item.name = name;
		item.type = ST_FLOAT;
		item.index = _floats.size();
		_floats.push_back(value);
		_items.push_back(item);
		_model.add(name, item);
	}

	// -------------------------------------------------------
	// set float
	// -------------------------------------------------------
	bool DynamicGameSettings::setFloat(const char* name, float value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*_floats[item.index] = value;
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// add color
	// -------------------------------------------------------
	void DynamicGameSettings::addColor(const char* name, Color* value, const Color& defaultValue) {
		*value = defaultValue;
		SettingsItem item;
		item.name = name;
		item.type = ST_COLOR;
		item.index = _colors.size();
		_colors.push_back(value);
		_items.push_back(item);
		_model.add(name, item);
	}

	// -------------------------------------------------------
	// set float
	// -------------------------------------------------------
	bool DynamicGameSettings::setColor(const char* name, const Color& value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*_colors[item.index] = value;
				return true;
			}
		}
		return false;
	}

	void DynamicGameSettings::addRect(const char* name, Rect* value, const Rect& defaultValue) {
		*value = defaultValue;
		SettingsItem item;
		item.name = name;
		item.type = ST_RECT;
		item.index = _rects.size();
		_rects.push_back(value);
		_items.push_back(item);
		_model.add(name, item);
	}

	bool DynamicGameSettings::setRect(const char* name, const Rect& value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*_rects[item.index] = value;
				return true;
			}
		}
		return false;
	}

	void DynamicGameSettings::addPath(const char* name, Vector2fPath* value) {
		SettingsItem item;
		item.name = name;
		item.type = ST_V2_PATH;
		item.index = _v2_paths.size();
		_v2_paths.push_back(value);
		_items.push_back(item);
		_model.add(name, item);
	}

	bool DynamicGameSettings::setPath(const char* name, const Vector2fPath& value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*_v2_paths[item.index] = value;
				return true;
			}
		}
		return false;
	}

	// -------------------------------------------------------
	// export json
	// -------------------------------------------------------
	bool DynamicGameSettings::saveData(JSONWriter& writer) {
		writer.startCategory("settings");
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (item.type == ST_FLOAT) {
				writer.write(item.name, *_floats[item.index]);
			}
			else if (item.type == ST_RECT) {
				writer.write(item.name, *_rects[item.index]);
			}
			else if (item.type == ST_V2_PATH) {
				writer.write(item.name, *_v2_paths[item.index]);
			}
			else if (item.type == ST_COLOR) {
				writer.write(item.name, *_colors[item.index]);
			}
		}
		writer.endCategory();
		return true;
	}

	// -------------------------------------------------------
	// import json
	// -------------------------------------------------------
	bool DynamicGameSettings::loadData(const JSONReader& loader) {
		int c = loader.find_category("settings");
		if (c != -1) {
			for (int i = 0; i < _items.size(); ++i) {
				const SettingsItem& item = _items[i];
				if (loader.contains_property(c,item.name)) {
					if (item.type == ST_FLOAT) {
						loader.get_float(c,item.name, _floats[item.index]);
					}
					else if (item.type == ST_INT) {
						loader.get_int(c,item.name, _ints[item.index]);
					}
					else if (item.type == ST_RECT) {
						loader.get_rect(c,item.name, _rects[item.index]);
					}
					else if (item.type == ST_V2_PATH) {
						loader.get_vec2_path(c, item.name, _v2_paths[item.index]);
					}
					else if (item.type == ST_COLOR) {
						loader.get_color(c, item.name, _colors[item.index]);
					}
				}
			}
		}
		return true;
	}

	// -------------------------------------------------------
	// show dialog
	// -------------------------------------------------------
	void DynamicGameSettings::showDialog(v2* pos) {
		gui::start(20, pos);
		if (gui::begin("Settings Dialog", &_state)) {
			gui::ComboBox(&_model, &_offset, 10);
			gui::beginGroup();
			if (gui::Button("Save")) {
				save();
			}
			if (gui::Button("Load")) {
				load();
			}
			gui::endGroup();
		}
		if (_model.hasSelection()) {
			SettingsItem element = _model.getSelectedValue();
			gui::Header(element.name);
			if (element.type == ST_FLOAT) {
				gui::InputFloat(element.name, _floats[element.index]);
			}
			else if (element.type == ST_INT) {
				gui::InputInt(element.name, _ints[element.index]);
			}
			else if (element.type == ST_RECT) {
				gui::InputRect(element.name, _rects[element.index]);
			}
		}
		gui::end();
	}

	// -------------------------------------------------------
	// show dialog
	// -------------------------------------------------------
	void DynamicGameSettings::showCompleteDialog(v2* pos) {
		gui::start(20, pos);
		if (gui::begin("Settings Dialog", &_state)) {
			for (int i = 0; i < _items.size(); ++i) {
				const SettingsItem& element = _items[i];
				if (element.type == ST_FLOAT) {
					gui::InputFloat(element.name, _floats[element.index]);
				}
				else if (element.type == ST_INT) {
					gui::InputInt(element.name, _ints[element.index]);
				}
				else if (element.type == ST_RECT) {
					gui::InputRect(element.name, _rects[element.index]);
				}
			}
			gui::beginGroup();
			if (gui::Button("Save")) {
				save();
			}
			if (gui::Button("Load")) {
				load();
			}
			gui::endGroup();
		}
		gui::end();
	}

}