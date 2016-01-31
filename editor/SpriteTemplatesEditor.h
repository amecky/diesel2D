#pragma once
#include "..\ui\IMGUI.h"
#include "AssetEditor.h"
#include "..\sprites\Sprite.h"

namespace ds {

	class SpriteTemplates;

	class SpriteTemplatesEditor : public AssetEditor {

	public:
		SpriteTemplatesEditor(SpriteTemplates* templates);
		~SpriteTemplatesEditor();
		void showDialog();
		void init();
		bool hasSelection() const;
		const Sprite& getSelection() const;
		Sprite& getSelection();
		bool showTexture() const {
			return _showTexture;
		}
		void moveOffset(const v2& m) {
			_textureOffset += m;
		}
		v2 getTexturePosition() const;
		void setTexturePosition(int x, int y);
		void setDimension(int x, int y);
		const char* getShortcut() const {
			return "SPT";
		}
	private:
		SpriteTemplates* _templates;
		int _state;
		gui::ComponentModel<int> _model;
		int _offset;
		gui::InputDialog _dialog;
		v2 _position;
		bool _showTexture;
		v2 _textureOffset;
		v2 _texturePos;
		v2 _dim;
	};

}