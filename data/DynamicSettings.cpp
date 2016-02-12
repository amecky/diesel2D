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
		item.ptr.iPtr = value;
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
				*item.ptr.iPtr = value;
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
		item.ptr.fPtr = value;
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
				*item.ptr.fPtr = value;
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
		item.ptr.cPtr = value;
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
				*item.ptr.cPtr = value;
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
		item.ptr.rPtr = value;
		_items.push_back(item);
		_model.add(name, item);
	}

	bool DynamicGameSettings::setRect(const char* name, const Rect& value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*item.ptr.rPtr = value;
				return true;
			}
		}
		return false;
	}

	void DynamicGameSettings::addPath(const char* name, Vector2fPath* value) {
		SettingsItem item;
		item.name = name;
		item.type = ST_V2_PATH;
		item.ptr.pPtr = value;
		_items.push_back(item);
		_model.add(name, item);
	}

	bool DynamicGameSettings::setPath(const char* name, const Vector2fPath& value) {
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (strcmp(item.name, name) == 0) {
				*item.ptr.pPtr = value;
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
			if (item.type == ST_INT) {				
				writer.write(item.name, *item.ptr.iPtr);
			}
			else if (item.type == ST_FLOAT) {
				writer.write(item.name, *item.ptr.fPtr);
			}
			else if (item.type == ST_RECT) {
				writer.write(item.name, *item.ptr.rPtr);
			}
			else if (item.type == ST_V2_PATH) {
				writer.write(item.name, *item.ptr.pPtr);
			}
			else if (item.type == ST_COLOR) {
				writer.write(item.name, *item.ptr.cPtr);
			}
			else if (item.type == ST_V2_PATH) {
				writer.write(item.name, *item.ptr.pPtr);
			}
		}
		writer.endCategory();
		return true;
	}

	// -------------------------------------------------------
	// import json
	// -------------------------------------------------------
	bool DynamicGameSettings::loadData(const JSONReader& loader) {
		StopWatch sw("DNG::loadData");
		int c = loader.find_category("settings");
		if (c != -1) {
			for (int i = 0; i < _items.size(); ++i) {
				const SettingsItem& item = _items[i];
				if (loader.contains_property(c,item.name)) {
					if (item.type == ST_FLOAT) {
						loader.get_float(c,item.name, item.ptr.fPtr);
					}
					else if (item.type == ST_INT) {				
						loader.get_int(c, item.name, item.ptr.iPtr);
					}
					else if (item.type == ST_RECT) {
						loader.get_rect(c, item.name, item.ptr.rPtr);
					}
					else if (item.type == ST_V2_PATH) {
						loader.get_vec2_path(c, item.name, item.ptr.pPtr);
					}
					else if (item.type == ST_COLOR) {
						loader.get_color(c, item.name, item.ptr.cPtr);
					}
				}
			}
		}
		return true;
	}

}