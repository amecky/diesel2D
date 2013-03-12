#include "DialogManager.h"
#include "..\utils\StringUtils.h"

namespace ds {

// -------------------------------------------------------
// Constructor
// -------------------------------------------------------
DialogManager::DialogManager(void) {
}

// -------------------------------------------------------
// Destructor - delete all dialogs
// -------------------------------------------------------
DialogManager::~DialogManager(void) {
	Dialogs::iterator it = m_Dialogs.begin();
	while ( it != m_Dialogs.end() ) {
		delete (*it);
		it = m_Dialogs.erase(it);
	}
}

// -------------------------------------------------------
// Add dialog to internal list
// -------------------------------------------------------
DialogID DialogManager::addDialog(GUIDialog* dialog) {
	uint32 idx = m_Dialogs.size();
	m_Dialogs.push_back(dialog);
	return idx;
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
	for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
		GUIDialog* dlg = m_Dialogs[i];
		if ( dlg->isActive() ) {
			dlg->render();
		}
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
				*dlgId = i;
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
	GUIDialog* oldDlg = findByName(oldDialogName);
	GUIDialog* newDlg = findByName(newDialogName);
	if ( oldDlg != 0 && newDlg != 0 ) {
		ToggleAction ta;
		ta.oldDialog = oldDlg;
		ta.newDialog = newDlg;
		ta.buttonId = buttonId;
		m_ToggleActions.push_back(ta);
	}
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
GUIDialog* DialogManager::findByName(const char* name) {
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

}