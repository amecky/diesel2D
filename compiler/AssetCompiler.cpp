#include "AssetCompiler.h"
#include "HUDConverter.h"
#include "..\io\FileWatcher.h"
#include "..\utils\Log.h"
#include "..\utils\FileUtils.h"
#include "GUIConverter.h"
#include "DialogConverter.h"
#include "BitmapFontConverter.h"
#include "ParticleSystemConverter.h"
#include "SpriteConverter.h"
#include "ParticleManagerConverter.h"
#include "..\objects\BloomComponent.h"
#include "SpritesDescriptionConverter.h"
#include "ScriptConverter.h"
#include "..\script\Tokenizer.h"
#include "BlueprintConverter.h"
#include "NewParticleSystemConverter.h"

namespace ds {

// -------------------------------------------------------
// Asset database
// -------------------------------------------------------
void AssetDB::readDB() {
	FILE* file = fopen("data\\asset.db", "rb");
	if (file) {
		int num = 0;
		fread(&num,sizeof(int),1,file);
		LOG << "DB entries: " << num;
		for ( int i = 0; i < num; ++i ) {
			AssetDBEntry entry;
			fread(&entry,sizeof(AssetDBEntry),1,file);
			LOG << "db entry " << entry.fileName;
			m_Entries.push_back(entry);
		}
		fclose(file);
	}
	else {
		LOGE << "No asset.db file found";
	}
}

void AssetDB::saveDB() {
	FILE* file = fopen("data\\asset.db", "wb");
	if (file) {
		int num = m_Entries.size();
		fwrite(&num, sizeof(int), 1, file);
		for ( int i = 0; i < num; ++i ) {
			AssetDBEntry& entry = m_Entries[i];
			fwrite(&entry,sizeof(AssetDBEntry),1,file);
		}
		fclose(file);
	}
}

bool AssetDB::contains(const char* fileName) {
	for ( size_t i = 0; i < m_Entries.size(); ++i ) {
		AssetDBEntry& entry = m_Entries[i];
		if ( strcmp(entry.fileName,fileName) == 0 ) {
			return true;
		}
	}
	return false;
}

void AssetDB::add(const char* name,FILETIME fileTime) {
	if ( !contains(name)) {
		AssetDBEntry entry;
		sprintf(entry.fileName,"%s",name);
		entry.fileTime = fileTime;
		m_Entries.push_back(entry);
	}
	else {
		update(name,fileTime);
	}
}

void AssetDB::update(const char* name,FILETIME fileTime) {
	for ( size_t i = 0; i < m_Entries.size(); ++i ) {
		AssetDBEntry& entry = m_Entries[i];
		if ( strcmp(entry.fileName,name) == 0 ) {
			entry.fileTime = fileTime;
		}
	}
}

bool AssetDB::hasChanged(const char* fileName) {
	if ( !contains(fileName)) {
		return true;
	}
	for ( size_t i = 0; i < m_Entries.size(); ++i ) {
		AssetDBEntry& entry = m_Entries[i];
		if ( strcmp(entry.fileName,fileName) == 0 ) {
			if ( file::compareFileTime(entry.fileName,entry.fileTime)) {
				LOGC("AssetDB") << "file " << entry.fileName << " has changed";
				file::getFileTime(entry.fileName,entry.fileTime);
				return true;
			}
		}
	}
	LOGC("AssetDB") << "file " << fileName << " has not changed";
	return false;
}

AssetCompiler::AssetCompiler() {
	m_Database.readDB();
	m_Mapping[CVT_HUD] = new HUDConverter;
	m_Mapping[CVT_GUI] = new GUIConverter;
	m_Mapping[CVT_DIALOG] = new DialogConverter;
	m_Mapping[CVT_FONT] = new BitmapFontConverter;
	m_Mapping[CVT_PARTICLESYSTEM] = new ParticleSystemConverter;
	m_Mapping[CVT_SPRITE] = new SpriteConverter;
	m_Mapping[CVT_PARTICLEMANAGER] = new ParticleManagerConverter;
	m_Mapping[CVT_BLOOM_COMPONENT] = new BloomComponentConverter;
	m_Mapping[CVT_SPRITES_DESCRIPTION] = new SpritesDescriptionConverter;
	m_Mapping[CVT_SCRIPT] = new ScriptConverter;
	m_Mapping[CVT_BLUEPRINT] = new BlueprintConverter;
	m_Mapping[CVT_NPS] = new NewParticleSystemConverter;
	m_CustomIndex = 100;
}


AssetCompiler::~AssetCompiler() {
	ConverterMapping::iterator it = m_Mapping.begin();
	while ( it != m_Mapping.end() ) {
		delete (*it).second;
		++it;
	}
	m_Database.saveDB();
}

void AssetCompiler::setWorld(World* world) {
	BlueprintConverter* cnv = (BlueprintConverter*)m_Mapping[CVT_BLUEPRINT];
	cnv->setWorld(world);
}

uint32 AssetCompiler::registerConverter(Converter* converter) {
	uint32 idx = m_CustomIndex;
	m_Mapping[idx] = converter;
	++m_CustomIndex;
	return idx;
}

void AssetCompiler::reload(FileWatch* watch) {
	BinaryLoader loader;
	int signature[] = {0,8,15};
	if ( loader.open(watch->binaryName,signature,3)) {
		//watch->serializer->load(loader);
		loader.close();
	}
}

uint32 AssetCompiler::getConvertedID(const char* name) {
	IdString hashName = string::murmur_hash(name);
	ConverterMapping::iterator it = m_Mapping.begin();
	while ( it != m_Mapping.end() ) {
		if ( it->second->getHashName() == hashName ) {
			return it->first;
		}
		++it;
	}
	return UINT32_MAX;
}

void AssetCompiler::load(const char* fileName,Serializer* serializer,uint32 type) {
	assert( type != UINT32_MAX );
	// check if we support the type
	assert(m_Mapping.find(type) != m_Mapping.end());
	Converter* converter = m_Mapping[type];
	char buffer[256];
	sprintf(buffer,"%s\\%s.json",converter->getResourceDirectory(),fileName);
	if ( file::fileExists(buffer) ) {
		IdString hash = string::murmur_hash(buffer);
#ifdef DEBUG
		if ( m_Database.hasChanged(buffer) ) {
			LOGC("AssetCompiler") << "converting file " << buffer;		
			// convert json to binary file
			converter->convert(fileName);	
			LOGC("AssetCompiler") << "file converted";
		}	
		FileWatch watch;
		watch.hash = hash;
		strcpy(watch.jsonName,buffer);
		strcpy(watch.binaryName,fileName);
		watch.serializer = serializer;
		watch.type = type;
		file::getFileTime(buffer,watch.fileTime);
		m_WatchList.push_back(watch);
		LOGC("AssetCompiler") << "file watch created";
		m_Database.add(buffer,watch.fileTime);
#endif
		// load file
		LOGC("AssetCompiler") << "loading binary file  - original name: " << buffer;
		BinaryLoader loader;
		sprintf(buffer,"data\\%u",hash);
		int signature[] = {0,8,15};
		LOGC("AssetCompiler") << "loading binary file: " << buffer;
		if ( loader.open(buffer,signature,3) == 0 ) {
			serializer->load(&loader);
		}
		loader.close();
	}
	else {
		LOGEC("AssetCompiler") << "Cannot find file: " << buffer;
	}
}

void AssetCompiler::loadScript(const char* fileName,Script* script) {
	char buffer[256];
	sprintf(buffer,"content\\scripts\\%s.script",fileName);
#ifdef DEBUG
	LOGC("AssetCompiler") << "converting script " << fileName;
	ScriptWatch watch;
	strcpy(watch.jsonName,buffer);
	strcpy(watch.binaryName,fileName);
	watch.script = script;
	file::getFileTime(fileName,watch.fileTime);
	m_ScriptWatchList.push_back(watch);
	// load file
	std::vector<ScriptBlock> blocks;
	script->tokenizer.loadScript(buffer,script->context,blocks);
	sprintf(buffer,"data\\scripts\\%s.dsb",fileName);
	script->tokenizer.saveByteCode(buffer,blocks,script->context);
#endif
	sprintf(buffer,"data\\scripts\\%s.dsb",fileName);
	script->hash = string::murmur_hash(fileName);
	script->tokenizer.loadByteCode(buffer,script->blocks,script->context);
}

void AssetCompiler::update() {
	for ( size_t i = 0; i < m_ScriptWatchList.size(); ++i ) {
		ScriptWatch* watch = &m_ScriptWatchList[i];
		if ( file::compareFileTime(watch->jsonName,watch->fileTime)) {
			m_Database.update(watch->jsonName,watch->fileTime);
			LOGC("AssetCompiler") << "Reloading file: " << watch->jsonName;	
			std::vector<ScriptBlock> blocks;
			char buffer[256];
			sprintf(buffer,"content\\scripts\\%s.script",watch->binaryName);
			watch->script->tokenizer.loadScript(buffer,watch->script->context,blocks);
			sprintf(buffer,"data\\scripts\\%s.dsb",watch->binaryName);
			watch->script->tokenizer.saveByteCode(buffer,blocks,watch->script->context);
			sprintf(buffer,"data\\scripts\\%s.dsb",watch->binaryName);
			// FIXME: reset script
			watch->script->tokenizer.loadByteCode(buffer,watch->script->blocks,watch->script->context);
			file::getFileTime(watch->jsonName,watch->fileTime);
		}
	}
	for ( size_t i = 0; i < m_WatchList.size(); ++i ) {
		FileWatch* watch = &m_WatchList[i];
		if ( file::compareFileTime(watch->jsonName,watch->fileTime)) {
			m_Database.update(watch->jsonName,watch->fileTime);
			LOGC("AssetCompiler") << "Reloading file: " << watch->jsonName;	
			assert(m_Mapping.find(watch->type) != m_Mapping.end());
			Converter* converter = m_Mapping[watch->type];
			// convert json to binary file
			converter->convert(watch->binaryName);
			BinaryLoader loader;
			char buffer[256];
			sprintf(buffer,"data\\%u",watch->hash);
			int signature[] = {0,8,15};
			if ( loader.open(buffer,signature,3) == 0 ) {
				watch->serializer->load(&loader);
				loader.close();
			}
			else {
				LOGEC("AssetCompiler") << "Cannot reload binary file " << buffer;
			}
			file::getFileTime(watch->jsonName,watch->fileTime);
		}
	}
}

}
