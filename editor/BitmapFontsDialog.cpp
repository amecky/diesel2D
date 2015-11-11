#include "BitmapFontsDialog.h"
#include "..\ui\IMGUI.h"
#include "..\DialogResources.h"
#include "..\renderer\graphics.h"

namespace ds {

	BitmapFontsDialog::BitmapFontsDialog() : _state(1) , _offset(0) , _position(512,384) {
	}

	BitmapFontsDialog::~BitmapFontsDialog() {
	}

	void BitmapFontsDialog::init() {
		std::vector<BitmapFont*> list = renderer::getBitmapFontList();
		for (size_t i = 0; i < list.size(); ++i) {
			_fontNames.add(list[i]->name, list[i]->name);
		}
	}

	void BitmapFontsDialog::show() {
		gui::start(DIALOG_MANAGER_ID, &_position);
		if (gui::begin("Bitmap Fonts", &_state)) {
			gui::ComboBox(DIALOG_MANAGER_ID + 1, &_fontNames, &_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 2, "Add")) {
				//_showAdd = true;
			}			
			gui::endGroup();
		}
		gui::end();
		
		if (_fontNames.hasSelection()) {
			const char* name = _fontNames.getSelectedValue();
			BitmapFont* font = renderer::getBitmapFont(name);
			if (font != 0) {
				if (gui::begin(font->name, &_state)) {
					gui::InputInt(DIALOG_MANAGER_ID + 4,"start char",&font->startChar);
					gui::InputInt(DIALOG_MANAGER_ID + 5, "end char", &font->endChar);
					gui::InputInt(DIALOG_MANAGER_ID + 6, "charHeight", &font->charHeight);
					gui::InputInt(DIALOG_MANAGER_ID + 7, "gridHeight", &font->gridHeight);
					gui::InputInt(DIALOG_MANAGER_ID + 8, "startX", &font->startX);
					gui::InputInt(DIALOG_MANAGER_ID + 9, "startY", &font->startY);
					gui::InputInt(DIALOG_MANAGER_ID + 10, "width", &font->width);
					gui::InputInt(DIALOG_MANAGER_ID + 11, "height", &font->height);
					gui::InputInt(DIALOG_MANAGER_ID + 12, "padding", &font->padding);
					gui::InputInt(DIALOG_MANAGER_ID + 13, "textureSize", &font->textureSize);
					gui::beginGroup();
					if (gui::Button(DIALOG_MANAGER_ID + 3, "Save")) {
						//_showAdd = true;
					}
					gui::endGroup();
				}
				gui::end();
			}
		}
	}

}
