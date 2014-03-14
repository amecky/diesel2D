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
	
	PR_END("SettingsManager-checkFiles")
}


}