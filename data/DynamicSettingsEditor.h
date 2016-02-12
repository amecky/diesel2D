#pragma once
#include "..\editor\AssetEditor.h"
#include "DynamicSettings.h"

namespace ds {

	class DynamicSettingsEditor : public AssetEditor {

	public:
		DynamicSettingsEditor(DynamicGameSettings* settings);
		~DynamicSettingsEditor();

		void showDialog();

		void init();

		const char* getShortcut() const {
			return "SET";
		}

	private:
		DynamicGameSettings* _settings;
		v2 _position;
		int _state;
	};

}

