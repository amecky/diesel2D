#pragma once
#include "..\dxstdafx.h"
#include "GUIDialog.h"
#include <vector>
#include "..\utils\PlainTextReader.h"
#include "..\io\Serializer.h"
#include "..\compiler\AssetCompiler.h"
#include "..\ui\IMGUI.h"

namespace ds {

class DialogManager : public Serializer {

typedef std::vector<GUIDialog*> Dialogs;

public:
	DialogManager(void);
	~DialogManager(void);	
	void init(const char* fontName,int textureID);
	void render();
	void activate(const char* dialogName);
	void deactivate(const char* dialogName);
	bool onButtonUp(int button,int x,int y,DialogID* dlgId,int* selected);
	void updateMousePos(const Vector2f& mousePos);
	GUIDialog* get(const char* dialogName);
	void load(BinaryLoader* loader);
	void tick(float dt);
	void showDialog();
private:	
	void setActiveFlag(const char* name,bool active);	
	void createDialog(const char* name,int id,GUIDialog* dialog);
	BitmapFont* m_Font;
	bool m_Initialized;
	Dialogs m_Dialogs;
	uint32 m_Index;

	gui::ComponentModel<GUIDialog*> _model;
	v2 _dialogPos;
	int _dialogState;
	int _offset;
};

}

