#pragma once
#include "..\dxstdafx.h"
#include "GUIDialog.h"
#include <vector>
#include "..\utils\PlainTextReader.h"
#include "..\io\Serializer.h"
#include "..\compiler\AssetCompiler.h"

namespace ds {

class DialogManager : public Serializer {

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
	void init(const char* fontName,int textureID);
	void render();
	void toggle(const char* oldDialogName,const char* newDialogName);
	void activate(const char* dialogName);
	void deactivate(const char* dialogName);
	bool onButtonDown(int button,int x,int y,DialogID* dlgId,int* selected);
	bool OnChar(char ascii,unsigned int keyState);
	void updateMousePos(const Vector2f& mousePos);
	GUIDialog* get(const char* dialogName);
	void addToggleAction(const char* oldDialogName,const char* newDialogName,int buttonId);
	void setAssetCompiler(AssetCompiler* assetCompiler) {
		m_AssetCompiler = assetCompiler;
	}
	//void activate(const char* name);
	//void deactivate(const char* name);
	void load(BinaryLoader* loader);
	void tick(float dt);
private:	
	void setActiveFlag(const char* name,bool active);	
	void createDialog(const char* name,int id,GUIDialog* dialog);
	AssetCompiler* m_AssetCompiler;
	BitmapFont* m_Font;
	bool m_Initialized;
	Dialogs m_Dialogs;
	uint32 m_Index;
	ToggleActions m_ToggleActions;
};

}

