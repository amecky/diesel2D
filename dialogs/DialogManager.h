#pragma once
#include "..\dxstdafx.h"
#include "GUIDialog.h"
#include "..\compiler\DataFile.h"
#include "..\ui\IMGUI.h"

namespace ds {

struct DialogDefinition {
	char name[32];
	IdString hash;
	GUIDialog* dialog;
};

typedef Array<DialogDefinition> Dialogs;

class DialogManager : public DataFile {

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
	GUIDialog* create(const char* dialogName);
	void tick(float dt);
	bool saveData(JSONWriter& writer);
	bool loadData(JSONReader& reader);
	const char* getFileName() const {
		return "resources\\gui.json";
	}
	const Dialogs& getDialogs() const {
		return m_Dialogs;
	}
	bool remove(const char* name);
	bool contains(const char* name) const;
private:		
	void clear();
	void setActiveFlag(const char* name,bool active);	
	void createDialog(const char* name,int id,GUIDialog* dialog);
	BitmapFont* m_Font;
	bool m_Initialized;
	Dialogs m_Dialogs;
	uint32 _index;
};

}

