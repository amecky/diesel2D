#include "SettingsManager.h"
#include "..\utils\FileUtils.h"
#include "..\utils\Profiler.h"
#include "..\pipeline\PAKReader.h"
#include "..\pipeline\PAKWriter.h"
#include "..\memory\DataBlockAllocator.h"

namespace ds {

SettingsManager::SettingsManager(void) {
}


SettingsManager::~SettingsManager(void) {
	// FIXME: delete all settings
	/*
	SpriteSettings::iterator it = m_Settings.begin();
	while ( it != m_Settings.end() ) {
		delete (*it).sprite;
		++it;
	}
	*/
}

bool SettingsManager::hasSettings(const char* fileName) {
	return getSettings(fileName) != 0;
}

DynamicSettings* SettingsManager::getSettings(const char* fileName) {
	IdString hashName = string::murmur_hash(fileName);
	for ( size_t i = 0; i < m_Files.size(); ++i ) {
		SettingsFile* file = &m_Files[i];
		if ( file->hashName == hashName ) {
			return file->settings;
		}
	}
	return 0;
}

void SettingsManager::loadSettings(const char* fileName,DynamicSettings* settings,bool check) {
	if ( hasSettings(fileName) && check ) {
		LOGC(logINFO,"SettingsManager") << "already loaded settings - file: " << fileName;
		settings = getSettings(fileName);
	}
	else {
		char buffer[256];
		sprintf(buffer,"content\\resources\\settings\\%s.settings",fileName);		
		SettingsFile file;
		file.settings = settings;
		file.hashName = string::murmur_hash(fileName);
		file.name = std::string(fileName);
		ds::file::getFileTime(buffer,file.fileTime);
		NewSettingsReader settingsReader;
		if ( settingsReader.parse(buffer)) {
			settings->load(settingsReader);
			m_Files.push_back(file);
			LOGC(logINFO,"SettingsManager") << "loading settings file: " << file.name;
		}
		else {
			LOGC(logINFO,"SettingsManager") << "Cannot parse settings files";
		}
	}
}

void SettingsManager::checkSettingsFiles() {
	PR_START("SettingsManager-checkFiles")
	char buffer[256];
	for ( size_t i = 0; i < m_Files.size(); ++i ) {
		SettingsFile* file = &m_Files[i];
		sprintf(buffer,"content\\resources\\settings\\%s.settings",file->name.c_str());
		if ( ds::file::compareFileTime(buffer,file->fileTime)) {
			LOGC(logINFO,"SettingsManager") << "reloading file: " << buffer;
			NewSettingsReader settingsReader;
			if ( settingsReader.parse(buffer)) {
				file->settings->load(settingsReader);
				ds::file::getFileTime(buffer,file->fileTime);
			}
		}		
	}
	reloadSpriteSettings();
	PR_END("SettingsManager-checkFiles")
}

// -------------------------------------------------------
// Reload settings if file has changed
// -------------------------------------------------------
void SettingsManager::reloadSpriteSettings() {
	PR_START("SettingsManager-reload")
	char buffer[256];
	for ( size_t i = 0; i < m_Settings.size(); ++i ) {
		SpriteSetting* setting = &m_Settings[i];
		sprintf(buffer,"content\\resources\\sprites\\%s.dss",setting->fileName);
		if ( file::compareFileTime(buffer,setting->fileTime)) {
			LOGC(logINFO,"SettingsManager") << "Reloading settings: " << setting->fileName;
			loadInternalSpriteSettings(setting);
		}
	}
	PR_END("SettingsManager-reload")
}

// -------------------------------------------------------
// Save binary version of setting to bss file
// -------------------------------------------------------
void SettingsManager::saveBinarySettings(SpriteSetting* setting) {
	char buffer[256];
	sprintf(buffer,"content\\data\\sprites\\%s.bss",setting->fileName);
	PAKWriter writer;
	Sprite* sprite = setting->sprite;
	writer.open(buffer);
	writer.writeVec2(sprite->position);
	writer.writeRect(sprite->textureRect);
	writer.writeFloat(sprite->rotation);
	writer.writeFloat(sprite->scaleX);
	writer.writeFloat(sprite->scaleY);
	writer.writeColor(sprite->color);
	writer.writeVec2(sprite->center);
	writer.close();
}

// -------------------------------------------------------
// Load settings from binary file
// -------------------------------------------------------
void SettingsManager::loadBinarySettings(SpriteSetting* setting) {
	char buffer[256];
	sprintf(buffer,"content\\data\\sprites\\%s.bss",setting->fileName);
	LOGC(logINFO,"SettingsManager") << "loading setting " << buffer;
	PAKReader reader;
	if ( reader.open(buffer)) {		
		Sprite* sprite = setting->sprite;
		sprite->position = reader.readVec2();
		sprite->textureRect = reader.readRect();
		sprite->rotation = reader.readFloat();		
		sprite->scaleX = reader.readFloat();
		sprite->scaleY = reader.readFloat();
		sprite->color = reader.readColor();
		sprite->center = reader.readVec2();
		file::getFileTime(buffer,setting->fileTime);
		reader.close();
	}
}

// -------------------------------------------------------
// Load settings from properties file
// -------------------------------------------------------
void SettingsManager::loadInternalSpriteSettings(SpriteSetting* setting) {
	char buffer[256];
	sprintf(buffer,"content\\resources\\sprites\\%s.dss",setting->fileName);
	LOGC(logINFO,"SettingsManager") << "loading setting " << buffer;
	NewSettingsReader sr;
	Sprite* sprite = setting->sprite;
	if ( sr.parse(buffer) ) {	
		sr.get<Vec2>("position",&sprite->position);
		sr.get<float>("rotation",&sprite->rotation);
		sr.get<Rect>("texture",&sprite->textureRect);
		sr.get<float>("scale_x",&sprite->scaleX);
		sr.get<float>("scale_y",&sprite->scaleY);
		sr.get<Color>("color",&sprite->color);	
		sr.get<Vec2>("center",&sprite->center);
		file::getFileTime(buffer,setting->fileTime);
		saveBinarySettings(setting);
	}
}

// -------------------------------------------------------
// Load settings
// -------------------------------------------------------
ds::Sprite* SettingsManager::loadSpriteSettings(const char* name) {
	Sprite* sprite = (Sprite*)gBlockMemory->alloc(sizeof(Sprite));//new Sprite;
	SpriteSetting iss;
	iss.sprite = sprite;
	iss.name = string::murmur_hash(name);
	strcpy(iss.fileName,name);
	loadInternalSpriteSettings(&iss);
	m_Settings.push_back(iss);
	return sprite;
}

// -------------------------------------------------------
// Has settings
// -------------------------------------------------------
bool SettingsManager::hasSpriteSettings(const char* name) {
	return getSpriteSettings(name) != 0;
}
// -------------------------------------------------------
// Get sprite by name
// -------------------------------------------------------
ds::Sprite* SettingsManager::getSpriteSettings(const char* name) {
	IdString hash = ds::string::murmur_hash(name);
	for ( size_t i = 0; i < m_Settings.size(); ++i ) {
		SpriteSetting* s = &m_Settings[i];
		if ( s->name == hash ) {
			return s->sprite;
		}
	}
	return 0;
}

}