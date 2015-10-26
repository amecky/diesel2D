#include "DialogManager.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\PlainTextReader.h"
#include "..\utils\FileUtils.h"
#include "..\compiler\Converter.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\DialogResources.h"

namespace ds {

	// -------------------------------------------------------
	// Constructor
	// -------------------------------------------------------
	DialogManager::DialogManager(void) : m_Index(0) , m_Initialized(false) {
		_dialogPos = v2(1050, 690);
		_dialogState = 1;
		_offset = 0;
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
			for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
				GUIDialog* dlg = m_Dialogs[i];
				if ( dlg->isActive() ) {
					dlg->render();
				}
			}
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
					return true;
				}
			}
		}
		return false;
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
				_model.add(dialogName.c_str(), dialog);

			}	
			loader->closeChunk();
		}		
	}

	void DialogManager::tick(float dt) {
		for (size_t i = 0; i < m_Dialogs.size(); ++i) {
			m_Dialogs[i]->tick(dt);
		}
	}

	void DialogManager::showDialog() {
		gui::start(DIALOG_MANAGER_ID, &_dialogPos);
		if (gui::begin("Dialogs", &_dialogState)) {
			gui::ComboBox(100, &_model, &_offset);
			if (gui::Button(101, "Activate")) {
				if (_model.hasSelection()) {
					GUIDialog* dlg = _model.getSelectedValue();
					dlg->activate();					
				}
			}
			if (gui::Button(103, "Deactivate")) {
				if (_model.hasSelection()) {
					GUIDialog* dlg = _model.getSelectedValue();
					dlg->deactivate();
				}
			}
		}
		gui::end();
		if (_model.hasSelection()) {
			GUIDialog* dlg = _model.getSelectedValue();
			dlg->showDialog();
		}
	}
}