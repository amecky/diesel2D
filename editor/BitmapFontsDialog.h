#pragma once
#include "..\ui\IMGUI.h"
#include "..\editor\AssetEditor.h"

namespace ds {

	class BitmapFontsDialog : public AssetEditor {

	public:
		BitmapFontsDialog();
		~BitmapFontsDialog();
		void showDialog();
		void init();
		const char* getShortcut() const {
			return "BMF";
		}
	private:
		gui::ComponentModel<const char*> _fontNames;
		int _offset;
		int _state;
		v2 _position;
	};

}

