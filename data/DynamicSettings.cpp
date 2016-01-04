#pragma once
#include "DynamicSettings.h"
#include "..\utils\JSONWriter.h"
#include "..\utils\PlainTextReader.h"
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

	// -------------------------------------------------------
	// export json
	// -------------------------------------------------------
	void DynamicGameSettings::save() {
		char buffer[64];
		sprintf_s(buffer, 64, "content\\%s.json", getName());
		JSONWriter jw;
		jw.open(buffer);
		jw.startCategory("settings");
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			if (item.type == ST_FLOAT) {
				jw.write(item.name, *_floats[item.index]);
			}
			else if (item.type == ST_RECT) {
				jw.write(item.name, *_rects[item.index]);
			}
		}
		jw.endCategory();
	}

	// -------------------------------------------------------
	// import json
	// -------------------------------------------------------
	void DynamicGameSettings::load() {
		char buffer[64];
		sprintf_s(buffer, 64, "content\\%s.json",getName());
		JSONReader reader;
		if (reader.parse(buffer)) {
			Category* c = reader.getCategory("settings");
			if (c != 0) {
				for (int i = 0; i < _items.size(); ++i) {
					const SettingsItem& item = _items[i];
					if (c->hasProperty(item.name)) {
						if (item.type == ST_FLOAT) {
							c->getFloat(item.name, _floats[item.index]);
						}
						else {
							if (item.type == ST_RECT) {
								c->getRect(item.name, _rects[item.index]);
							}
						}
					}
				}
			}
		}
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