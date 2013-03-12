#pragma once
#include "..\dxstdafx.h"
#include "GUIDialog.h"
#include <vector>

namespace ds {

typedef uint32 DialogID;

class DialogManager {

struct ToggleAction {
	GUIDialog* oldDialog;
	GUIDialog* newDialog;
	int buttonId;
};	

typedef std::vector<GUIDialog*> Dialogs;
typedef std::vector<ToggleAction> ToggleActions;

public:
	DialogManager(void);
	~DialogManager(void);
	DialogID addDialog(GUIDialog* dialog);
	void render();
	void toggle(const char* oldDialogName,const char* newDialogName);
	void activate(const char* dialogName);
	void deactivate(const char* dialogName);
	bool onButtonDown(int button,int x,int y,DialogID* dlgId,int* selected);
	bool OnChar(char ascii,unsigned int keyState);
	void updateMousePos(const Vec2& mousePos);
	GUIDialog* findByName(const char* dialogName);
	void addToggleAction(const char* oldDialogName,const char* newDialogName,int buttonId);
private:
	void setActiveFlag(const char* name,bool active);
	Dialogs m_Dialogs;
	ToggleActions m_ToggleActions;
};

}

