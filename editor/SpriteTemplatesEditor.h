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
	private:
		SpriteTemplates* _templates;
		int _state;
		gui::ComponentModel<int> _model;
		int _offset;
		gui::InputDialog _dialog;
		v2 _position;
	};

}