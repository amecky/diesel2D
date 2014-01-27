#include "DialogManager.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\FileUtils.h"

namespace ds {

// -------------------------------------------------------
// Constructor
// -------------------------------------------------------
DialogManager::DialogManager(void) : m_Index(0) , m_Initialized(false) {
}

// -------------------------------------------------------
// Destructor - delete all dialogs
// -------------------------------------------------------
DialogManager::~DialogManager(void) {
	Dialogs::iterator it = m_Dialogs.begin();
	while ( it != m_Dialogs.end()) {
		delete (*it);
		it = m_Dialogs.erase(it);
	}
}

// -------------------------------------------------------
// Init
// -------------------------------------------------------
void DialogManager::init(SpriteBatch* spriteBatch,Renderer* renderer,const char* fontName,int textureID) {
	m_SpriteBatch = spriteBatch;
	font::load(fontName,renderer,textureID,m_Font);
	m_Initialized = true;
}

// -------------------------------------------------------
// Create dialog
// -------------------------------------------------------
void DialogManager::createDialog(const char* name,int id,GUIDialog* dialog) {
	assert(get(name) == 0);
	dialog->init(name,id,m_SpriteBatch,&m_Font);
	++m_Index;
	m_Dialogs.push_back(dialog);
}

// -------------------------------------------------------
// Toggle the state of two dialogs
// -------------------------------------------------------
void DialogManager::toggle(const char* oldDialogName,const char* newDialogName) {
	deactivate(oldDialogName);
	activate(newDialogName);
}
// -------------------------------------------------------
// Activate dialog
// -------------------------------------------------------
void DialogManager::activate(const char* dialogName) {
	setActiveFlag(dialogName,true);
}

// -------------------------------------------------------
// Deactivate dialog
// -------------------------------------------------------
void DialogManager::deactivate(const char* dialogName) {
	setActiveFlag(dialogName,false);
}

// -------------------------------------------------------
// Set active flag for dialog
// -------------------------------------------------------
void DialogManager::setActiveFlag(const char* name,bool active) {
	IdString hashName = string::murmur_hash(name);
	for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
		GUIDialog* dlg = m_Dialogs[i];
		if ( dlg->getHashName() == hashName ) {
			if ( active ) {
				dlg->activate();
			}
			else {
				dlg->deactivate();
			}
		}
	}
}

// -------------------------------------------------------
// Render all active dialogs
// -------------------------------------------------------
void DialogManager::render() {
	if ( m_Initialized && !m_Dialogs.empty() ) {
		m_SpriteBatch->begin();
		for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
			GUIDialog* dlg = m_Dialogs[i];
			if ( dlg->isActive() ) {
				dlg->render();
			}
		}
		m_SpriteBatch->end();
	}
}

// -------------------------------------------------------
// On button down
// -------------------------------------------------------
bool DialogManager::onButtonDown(int button,int x,int y,DialogID* dlgId,int* selected) {
	for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
		GUIDialog* dlg = m_Dialogs[i];
		if ( dlg->isActive() ) {
			int ret = dlg->onButton(button,x,y,true);
			if ( ret != -1 ) {
				*dlgId = dlg->getDialogID();
				*selected = ret;
				// check if we have a toggleAction
				for ( size_t j = 0; j < m_ToggleActions.size(); ++j ) {
					ToggleAction* ta = &m_ToggleActions[j];
					if ( ta->buttonId == ret && ta->oldDialog->getHashName() == dlg->getHashName() ) {
						ta->oldDialog->deactivate();
						ta->newDialog->activate();
					}
				}
				return true;
			}
		}
	}
	return false;
}

// -------------------------------------------------------
// Add direct toggle action
// -------------------------------------------------------
void DialogManager::addToggleAction(const char* oldDialogName,const char* newDialogName,int buttonId) {
	GUIDialog* oldDlg = get(oldDialogName);
	GUIDialog* newDlg = get(newDialogName);
	assert(oldDlg != 0);
	assert(newDlg != 0);
	ToggleAction ta;
	ta.oldDialog = oldDlg;
	ta.newDialog = newDlg;
	ta.buttonId = buttonId;
	m_ToggleActions.push_back(ta);	
}

// -------------------------------------------------------
// Update mouse position
// -------------------------------------------------------
void DialogManager::updateMousePos(const Vec2& mousePos) {
	for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
		GUIDialog* dlg = m_Dialogs[i];
		if ( dlg->isActive() ) {
			dlg->updateMousePos(mousePos);			
		}
	}
}

// -------------------------------------------------------
// Find dialog by name
// -------------------------------------------------------
GUIDialog* DialogManager::get(const char* name) {
	IdString hashName = string::murmur_hash(name);
	for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
		GUIDialog* dlg = m_Dialogs[i];
		if ( dlg->getHashName() == hashName ) {
			return dlg;				
		}
	}
	return 0;
}

// -------------------------------------------------------
// OnChar
// -------------------------------------------------------
bool DialogManager::OnChar(char ascii,unsigned int keyState) {
	for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
		GUIDialog* dlg = m_Dialogs[i];
		if ( dlg->OnChar(ascii) ) {
			return true;
		}
	}
	return false;
}

bool DialogManager::loadDialogFromJSON(const char* dialogName,const char* name,int id) {
	char buffer[256];
	sprintf(buffer,"content\\dialogs\\%s.json",name);
	if ( file::fileExists(buffer)) {
		GUIDialog* dialog = new GUIDialog();
		createDialog(dialogName,id,dialog);
		dialog->load(name);
		return true;
	}	
	else {
		LOG(logINFO) << "File '" << name << "' not found";
	}
	return false;
}

}