#include "DialogManager.h"
#include "..\utils\StringUtils.h"
#include "..\utils\font.h"
#include "..\utils\FileUtils.h"
#include "..\sprites\SpriteBatch.h"
#include "..\renderer\graphics.h"
#include "..\DialogResources.h"

namespace ds {

	// -------------------------------------------------------
	// Constructor
	// -------------------------------------------------------
	DialogManager::DialogManager(void) : _index(1) , m_Initialized(false) {
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
			it = m_Dialogs.remove(it);
		}
	}

	// -------------------------------------------------------
	// Init
	// -------------------------------------------------------
	void DialogManager::init(BitmapFont* font) {
		m_Font = font;
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
		++_index;
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
			LOGE << "Cannot activate/deactivate '" << name << "'";
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

	bool DialogManager::contains(const char* name) const {
		IdString hashName = string::murmur_hash(name);
		for (size_t i = 0; i < m_Dialogs.size(); ++i) {
			const DialogDefinition& def = m_Dialogs[i];
			if (def.hash == hashName){
				return true;
			}
		}
		return false;
	}

	GUIDialog* DialogManager::create(const char* dialogName) {
		DialogDefinition def;
		strcpy(def.name, dialogName);
		def.hash = string::murmur_hash(dialogName);
		GUIDialog* dialog = new GUIDialog();
		LOG << "Creating new dialog: " << dialogName;
		def.dialog = dialog;
		dialog->init(def.name, _index, m_Font);
		m_Dialogs.push_back(def);
		++_index;
		return dialog;
	}
	// -------------------------------------------------------
	// Find dialog by name
	// -------------------------------------------------------
	GUIDialog* DialogManager::get(const char* name) {
		IdString hashName = string::murmur_hash(name);
		for ( size_t i = 0; i < m_Dialogs.size(); ++i) {
			const DialogDefinition& def = m_Dialogs[i];	
			if (def.hash == hashName){
				return def.dialog;
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
				it = m_Dialogs.remove(it);
				return true;
			}
			else {
				++it;
			}
		}
		return false;
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
	// export json
	// -------------------------------------------------------
	bool DialogManager::saveData(JSONWriter& writer) {
		return true;
	}

	bool DialogManager::loadData(const JSONReader& reader) {
		clear();
		_index = 1;
		int cats[32];
		int num = reader.get_categories(cats, 32);
		for (int i = 0; i < num; ++i ) {
			int id = 0;
			reader.get_int(cats[i],"id",&id);
			const char* name = reader.get_string(cats[i], "file");
			GUIDialog* dialog = new GUIDialog();
			LOG << "Creating new dialog: " << name;
			DialogDefinition def;
			sprintf_s(def.name, 32, "%s", name);
			def.hash = string::murmur_hash(name);
			def.dialog = dialog;
			dialog->init(def.name, id, m_Font);
			m_Dialogs.push_back(def);
		}
		return true;
	}
	
}