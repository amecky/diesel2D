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
		_showAdd = false;
	}

	// -------------------------------------------------------
	// Destructor - delete all dialogs
	// -------------------------------------------------------
	DialogManager::~DialogManager(void) {
		clear();
	}

	void DialogManager::clear() {
		Dialogs::iterator it = m_Dialogs.begin();
		while ( it != m_Dialogs.end()) {
			delete it->dialog;
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
		DialogDefinition def;
		def.dialog = dialog;
		def.hash = string::murmur_hash(name);
		strncpy(def.name, name, 32);
		dialog->init(name,id,m_Font);
		++m_Index;
		m_Dialogs.push_back(def);
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
			const DialogDefinition& def = m_Dialogs[i];
			if ( def.hash == hashName ) {
				found = true;
				if ( active ) {
					def.dialog->activate();
				}
				else {
					def.dialog->deactivate();
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
				const DialogDefinition& def = m_Dialogs[i];
				GUIDialog* dlg = def.dialog;
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
			const DialogDefinition& def = m_Dialogs[i];
			GUIDialog* dlg = def.dialog;
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
			const DialogDefinition& def = m_Dialogs[i];
			GUIDialog* dlg = def.dialog;
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
			const DialogDefinition& def = m_Dialogs[i];
			GUIDialog* dlg = def.dialog;
			if ( dlg->getHashName() == hashName ) {
				return dlg;				
			}
		}
		return 0;
	}

	// -------------------------------------------------------
	// remove dialog by name
	// -------------------------------------------------------
	bool DialogManager::remove(const char* name) {
		IdString hashName = string::murmur_hash(name);
		Dialogs::iterator it = m_Dialogs.begin();
		while (it != m_Dialogs.end()) {
			if (it->hash == hashName) {
				delete it->dialog;
				it = m_Dialogs.erase(it);
				return true;
			}
			else {
				++it;
			}
		}
		return false;
	}
	// -------------------------------------------------------
	// old load
	// -------------------------------------------------------
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
				LOGC("DialogManager") << "=> Dialog: " << dialogName;
				LOGC("DialogManager") << "name hash: " << string::murmur_hash(dialogName.c_str());
				LOGC("DialogManager") << "file hash: " << string::murmur_hash(file.c_str());
			}	
			loader->closeChunk();
		}		
	}

	// -------------------------------------------------------
	// tick
	// -------------------------------------------------------
	void DialogManager::tick(float dt) {
		for (size_t i = 0; i < m_Dialogs.size(); ++i) {
			const DialogDefinition& def = m_Dialogs[i];
			GUIDialog* dlg = def.dialog;
			if (dlg->isActive()) {
				dlg->tick(dt);
			}
		}
	}

	// -------------------------------------------------------
	// show dialog
	// -------------------------------------------------------
	void DialogManager::showDialog() {
		gui::start(DIALOG_MANAGER_ID, &_dialogPos);
		if (gui::begin("Dialogs", &_dialogState)) {
			gui::ComboBox(DIALOG_MANAGER_ID + 1, &_model, &_offset);
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 2, "Activate")) {
				if (_model.hasSelection()) {
					GUIDialog* dlg = _model.getSelectedValue().dialog;
					dlg->activate();					
				}
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 3, "Deactivate")) {
				if (_model.hasSelection()) {
					GUIDialog* dlg = _model.getSelectedValue().dialog;
					dlg->deactivate();
				}
			}
			gui::endGroup();
			gui::beginGroup();
			if (gui::Button(DIALOG_MANAGER_ID + 4, "Load")) {
				load();
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 5, "Save")) {
				save();
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 6, "Add")) {
				_showAdd = true;
			}
			else if (gui::Button(DIALOG_MANAGER_ID + 7, "Remove")) {
				if (_model.hasSelection()) {
					const DialogDefinition& def = _model.getSelectedValue();
					if (remove(def.name)) {
						_model.remove(_model.getSelection());
						_model.select(-1);
					}
				}
			}
			gui::endGroup();
		}
		gui::end();
		if (_showAdd) {
			int ret = _dialog.show("Please provide a name", "Name");
			if (ret == 1) {
				DialogDefinition def;
				strcpy(def.name,_dialog.getText());
				LOGC("DialogManager") << "dialog definition: " << def.name;
				//assert(get(def.name) == 0);
				if (get(def.name) == 0) {
					def.hash = string::murmur_hash(def.name);
					GUIDialog* dialog = new GUIDialog();
					LOGC("DialogManager") << "Creating new dialog: " << def.name;
					def.dialog = dialog;
					dialog->init(def.name, -1, m_Font);
					m_Dialogs.push_back(def);
					_model.add(def.name, def);
				}
				_showAdd = false;
			}
			if (ret == 2) {
				_showAdd = false;
			}
		}
		if (_model.hasSelection()) {
			GUIDialog* dlg = _model.getSelectedValue().dialog;
			dlg->showDialog();
		}
	}

	// -------------------------------------------------------
	// save
	// -------------------------------------------------------
	void DialogManager::save() {
		IdString hash = string::murmur_hash("gui_dialogs");
		char buffer[64];
		sprintf(buffer, "assets\\%u", hash);
		BinaryWriter writer;
		int signature[] = { 0, 8, 15 };
		writer.open(buffer, signature, 3);
		for (size_t i = 0; i < m_Dialogs.size(); ++i) {
			const DialogDefinition& def = m_Dialogs[i];
			writer.startChunk(CHNK_DLG_DEF, 1);
			writer.write(def.name);
			writer.write(def.hash);
			writer.closeChunk();			
		}
		writer.close();
	}

	// -------------------------------------------------------
	// load
	// -------------------------------------------------------
	void DialogManager::load() {
		_model.clear();
		clear();
		BinaryLoader loader;
		char buffer[64];
		IdString hash = string::murmur_hash("gui_dialogs");
		sprintf(buffer, "assets\\%u", hash);
		LOGC("DialogManager") << "reading file: " << buffer;
		int signature[] = { 0, 8, 15 };
		loader.open(buffer, signature, 3);
		while (loader.openChunk() == 0) {
			if (loader.getChunkID() == CHNK_DLG_DEF) {
				DialogDefinition def;
				loader.read(def.name);
				LOGC("DialogManager") << "dialog definition: " << def.name;
				//assert(get(def.name) == 0);
				if (get(def.name) == 0) {
					loader.read(&def.hash);
					GUIDialog* dialog = new GUIDialog();
					LOGC("DialogManager") << "Creating new dialog: " << def.name;
					def.dialog = dialog;
					dialog->init(def.name, -1, m_Font);
					m_Dialogs.push_back(def);
					_model.add(def.name, def);
				}
			}
			loader.closeChunk();
		}
	}
}