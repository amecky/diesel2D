#pragma once
#include "..\dxstdafx.h"
#include "GUIDialog.h"
#include <vector>

namespace ds {

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
	void init(SpriteBatch* spriteBatch,Renderer* renderer,const char* fontName,int textureID);
	DialogID addDialog(GUIDialog* dialog);
	void render();
	//GUIDialog* createDialog(const char* name);
	void createDialog(const char* name,GUIDialog* dialog);
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
	SpriteBatch* m_SpriteBatch;
	BitmapFont m_Font;
	bool m_Initialized;
	Dialogs m_Dialogs;
	uint32 m_Index;
	ToggleActions m_ToggleActions;
};

}

