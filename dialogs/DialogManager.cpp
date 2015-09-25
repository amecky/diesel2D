#include "DialogManager.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\FileUtils.h"
#include "..\compiler\Converter.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"

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
	void DialogManager::init(const char* fontName,int textureID) {
		m_Font = renderer::loadBitmapFont(fontName,textureID);	
		m_Initialized = true;
	}

	// -------------------------------------------------------
	// Create dialog
	// -------------------------------------------------------
	void DialogManager::createDialog(const char* name,int id,GUIDialog* dialog) {
		assert(get(name) == 0);
		dialog->init(name,id,m_Font);
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
		bool found = false;
		for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
			GUIDialog* dlg = m_Dialogs[i];
			if ( dlg->getHashName() == hashName ) {
				found = true;
				if ( active ) {
					dlg->activate();
				}
				else {
					dlg->deactivate();
				}
			}
		}
		if ( !found ) {
			LOGEC("DialogManager") << "Cannot activate/deactivate '" << name << "'";
		}
	}

	// -------------------------------------------------------
	// Render all active dialogs
	// -------------------------------------------------------
	void DialogManager::render() {
		if ( m_Initialized && !m_Dialogs.empty() ) {
			//m_SpriteBatch->begin();
			for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
				GUIDialog* dlg = m_Dialogs[i];
				if ( dlg->isActive() ) {
					dlg->render();
				}
			}
			//m_SpriteBatch->end();
		}
	}

	// -------------------------------------------------------
	// On button down
	// -------------------------------------------------------
	bool DialogManager::onButtonUp(int button,int x,int y,DialogID* dlgId,int* selected) {
		for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
			GUIDialog* dlg = m_Dialogs[i];
			if ( dlg->isActive() ) {
				int ret = dlg->onButton(button,x,renderer::getScreenHeight() - y,true);
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
	void DialogManager::updateMousePos(const Vector2f& mousePos) {
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

	void DialogManager::load(BinaryLoader* loader) {
		while ( loader->openChunk() == 0 ) {		
			if ( loader->getChunkID() == CHNK_GUI ) {	
				int textureID = 0;
				loader->read(&textureID);
				std::string str;
				loader->read(str);
				int max = 1024;
				loader->read(&max);
				init(str.c_str(),textureID);
			}	
			else if ( loader->getChunkID() == CHNK_DLG_DEF ) {	
				std::string dialogName;
				loader->read(dialogName);
				int id = 0;
				loader->read(&id);
				std::string file;
				loader->read(file);

				GUIDialog* dialog = new GUIDialog();
				LOGC("DialogManager") << "Creating new dialog: " << dialogName;
				createDialog(dialogName.c_str(),id,dialog);
				ds::assets::load(file.c_str(),dialog,CVT_DIALOG);

			}	
			loader->closeChunk();
		}		
	}

	void DialogManager::tick(float dt) {
		for (size_t i = 0; i < m_Dialogs.size(); ++i) {
			m_Dialogs[i]->tick(dt);
		}
	}
}