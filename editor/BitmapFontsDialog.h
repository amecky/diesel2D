#pragma once
#include "..\ui\IMGUI.h"

namespace ds {

	class BitmapFontsDialog {

	public:
		BitmapFontsDialog();
		~BitmapFontsDialog();
		void show();
		void init();
	private:
		gui::ComponentModel<const char*> _fontNames;
		int _offset;
		int _state;
		v2 _position;
	};

}

