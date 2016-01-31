#include "BitmapFontsDialog.h"
#include "..\ui\IMGUI.h"
#include "..\DialogResources.h"
#include "..\renderer\graphics.h"

namespace ds {

	BitmapFontsDialog::BitmapFontsDialog() : AssetEditor("BitmapFonts",0) , _state(1), _offset(0), _position(512, 384) {
	}

	BitmapFontsDialog::~BitmapFontsDialog() {
	}

	void BitmapFontsDialog::init() {
		const Array<BitmapFont*>& list = renderer::getBitmapFontList();
		for (size_t i = 0; i < list.size(); ++i) {
			_fontNames.add(list[i]->name, list[i]->name);
		}
	}

	void BitmapFontsDialog::showDialog() {
		gui::start(DIALOG_MANAGER_ID, &_position);
		if (gui::begin("Bitmap Fonts", &_state)) {
			gui::ComboBox(&_fontNames, &_offset);
			gui::beginGroup();
			if (gui::Button("Add")) {
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
					gui::InputInt("start char",&font->startChar);
					gui::InputInt("end char", &font->endChar);
					gui::InputInt("charHeight", &font->charHeight);
					gui::InputInt("gridHeight", &font->gridHeight);
					gui::InputInt("startX", &font->startX);
					gui::InputInt("startY", &font->startY);
					gui::InputInt("width", &font->width);
					gui::InputInt("height", &font->height);
					gui::InputInt("padding", &font->padding);
					gui::InputInt("textureSize", &font->textureSize);
					gui::beginGroup();
					if (gui::Button("Save")) {
						//_showAdd = true;
					}
					gui::endGroup();
				}
				gui::end();
			}
		}
	}

}
