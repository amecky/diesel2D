#include "FileWatcher.h"
#include "..\utils\FileUtils.h"
#include "..\utils\Log.h"
#include "..\utils\StringUtils.h"

namespace ds {

FileWatcher* gFileWatcher;

FileWatcher::FileWatcher() {
}


FileWatcher::~FileWatcher() {
}

void FileWatcher::update() {
	for ( size_t i = 0; i < m_WatchList.size(); ++i ) {
		FileWatch* watch = &m_WatchList[i];
		if ( file::compareFileTime(watch->filename,watch->fileTime)) {
			LOGC(logINFO,"FileWatcher") << "Reloading file: " << watch->filename;		
			watch->serializer->reload(watch->filename);
			file::getFileTime(watch->filename,watch->fileTime);
		}
	}
}

void FileWatcher::registerFile(const char* fileName,Serializer* serializer) {
	LOGC(logINFO,"FileWatcher") << "Registering file " << fileName << " to be monitored";	
	if ( !contains(fileName)) {
		FileWatch watch;
		IdString hash = string::murmur_hash(fileName);
		watch.hashName = hash;
		strcpy(watch.filename,fileName);
		watch.serializer = serializer;
		file::getFileTime(fileName,watch.fileTime);
		m_WatchList.push_back(watch);
	}
}

bool FileWatcher::contains(const char* fileName) {
	IdString hash = string::murmur_hash(fileName);
	for ( size_t i = 0; i < m_WatchList.size(); ++i ) {
		FileWatch* w = &m_WatchList[i];
		if ( w->hashName == hash ) {
			return true;
		}
	}
	return false;
}

}
