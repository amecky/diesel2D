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

struct DialogDefinition {
	char name[32];
	IdString hash;
	GUIDialog* dialog;
};

typedef std::vector<DialogDefinition> Dialogs;

public:
	DialogManager(void);
	~DialogManager(void);	
	void init(BitmapFont* font);
	void render();
	void activate(const char* dialogName);
	void deactivate(const char* dialogName);
	bool onButtonUp(int button,int x,int y,DialogID* dlgId,int* selected);
	void updateMousePos(const Vector2f& mousePos);
	GUIDialog* get(const char* dialogName);
	void load(BinaryLoader* loader);
	void tick(float dt);
	void showDialog();
	void load();
	void save();
private:	
	bool remove(const char* name);
	void clear();
	void setActiveFlag(const char* name,bool active);	
	void createDialog(const char* name,int id,GUIDialog* dialog);
	BitmapFont* m_Font;
	bool m_Initialized;
	Dialogs m_Dialogs;
	uint32 _index;

	gui::ComponentModel<DialogDefinition> _model;
	v2 _dialogPos;
	int _dialogState;
	int _offset;
	gui::InputDialog _dialog;
	bool _showAdd;

};

}

