#pragma once
#include "DynamicSettings.h"
#include "..\utils\JSONWriter.h"
#include "..\utils\PlainTextReader.h"
#include "..\DialogResources.h"
#include "..\io\BinaryLoader.h"
#include "..\io\BinaryWriter.h"

namespace ds {

	DynamicGameSettings::DynamicGameSettings() : _state(1), _offset(0) {}

	DynamicGameSettings::~DynamicGameSettings() {}

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
	void DynamicGameSettings::exportJSON() {
		char buffer[64];
		sprintf(buffer, "content\\settings.json");
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
	void DynamicGameSettings::importJSON() {
		char buffer[64];
		sprintf(buffer, "content\\settings.json");
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
	// load
	// -------------------------------------------------------
	void DynamicGameSettings::load() {
		BinaryLoader loader;
		char buffer[64];
		sprintf(buffer, "assets\\%u", string::murmur_hash("settings"));
		LOGC("DynamicGameSettings") << "loading file: " << buffer;
		int signature[] = { 0, 8, 15 };
		if (loader.open(buffer, signature, 3) == IO_OK) {
			while (loader.openChunk() == 0) {
				if (loader.getChunkID() == 1) {
					loader.read(buffer);
					int type = -1;
					loader.read(&type);
					if (type == ST_FLOAT) {
						float v = 0.0f;
						loader.read(&v);
						setFloat(buffer, v);
					}
					else if (type == ST_RECT) {
						Rect r;
						loader.read(&r);
						setRect(buffer, r);
					}
				}
				loader.closeChunk();
			}
		}
		else {
			LOGC("DynamicGameSettings") << "Error loading file";
		}
	}

	// -------------------------------------------------------
	// save
	// -------------------------------------------------------
	void DynamicGameSettings::save() {
		char buffer[64];
		sprintf(buffer, "assets\\%u", string::murmur_hash("settings"));
		BinaryWriter writer;
		int signature[] = { 0, 8, 15 };
		writer.open(buffer, signature, 3);
		for (int i = 0; i < _items.size(); ++i) {
			const SettingsItem& item = _items[i];
			writer.startChunk(1, 1);
			writer.write(item.name);
			writer.write(item.type);
			switch (item.type) {
				case ST_FLOAT: writer.write(*_floats[item.index]); break;
				case ST_RECT: writer.write(*_rects[item.index]); break;
			}
			writer.closeChunk();
		}
		writer.close();
	}

	// -------------------------------------------------------
	// show dialog
	// -------------------------------------------------------
	void DynamicGameSettings::showDialog(v2* pos) {
		gui::start(20, pos);
		if (gui::begin("Settings Dialog", &_state)) {
			gui::ComboBox(DGS_DIALOG_ID + 1, &_model, &_offset, 10);
			gui::beginGroup();
			if (gui::Button(DGS_DIALOG_ID + 2, "Save")) {
				save();
			}
			if (gui::Button(DGS_DIALOG_ID + 11, "Load")) {
				load();
			}
			if (gui::Button(DGS_DIALOG_ID + 14, "Export")) {
				exportJSON();
			}
			if (gui::Button(DGS_DIALOG_ID + 15, "Import")) {
				importJSON();
			}
			gui::endGroup();
		}
		gui::end();
		if (_model.hasSelection()) {
			if (gui::begin("Setting", &_state)) {
				SettingsItem element = _model.getSelectedValue();
				if (element.type == ST_FLOAT) {
					gui::InputFloat(DGS_DIALOG_ID + 4, element.name, _floats[element.index]);
				}
				else if (element.type == ST_RECT) {
					gui::InputRect(DGS_DIALOG_ID + 4, element.name, _rects[element.index]);
				}
			}
			gui::end();
		}
	}

}