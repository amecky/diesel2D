#include "AssetCompiler.h"
#include "HUDConverter.h"
#include "..\io\FileWatcher.h"
#include "..\utils\Log.h"
#include "..\utils\FileUtils.h"
#include "GUIConverter.h"
#include "DialogConverter.h"
#include "BitmapFontConverter.h"
#include "SpriteConverter.h"
#include "ParticleManagerConverter.h"
#include "..\objects\BloomComponent.h"
#include "NewParticleSystemConverter.h"
#include "..\renderer\TextureLoader.h"
#include "PathConverter.h"
#include "StraightPathConverter.h"
#include "DescriptorConverter.h"
#include "MeshConverter.h"
#include "ShaderConverter.h"
#include "..\particles\ParticleManager.h"

//const unsigned int UINT32_MAX = 1024;

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
			file::logFileTime(entry.fileTime);
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
		LOGC("AssetDB") << "file " << fileName << " is not yet in the database - loading";
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
	m_Mapping[CVT_SPRITE] = new SpriteConverter;
	m_Mapping[CVT_PARTICLEMANAGER] = new ParticleManagerConverter;
	m_Mapping[CVT_BLOOM_COMPONENT] = new BloomComponentConverter;
	m_Mapping[CVT_NPS] = new NewParticleSystemConverter;
	m_Mapping[CVT_PATH] = new PathConverter;
	m_Mapping[CVT_STRAIGHT_PATH] = new StraightPathConverter;
	m_Mapping[CVT_DESCRIPTOR] = new DescriptorConverter;
	m_Mapping[CVT_MESH] = new MeshConverter;
	m_Mapping[CVT_SHADER] = new ShaderConverter;
	m_BinarySerializerMapping[BINL_TEXTURE] = new TextureLoader;
	m_CustomIndex = 100;
}


AssetCompiler::~AssetCompiler() {
	LOG << "Removing asset compiler";
	ConverterMapping::iterator it = m_Mapping.begin();
	while ( it != m_Mapping.end() ) {
		delete (*it).second;
		++it;
	}
	BinarySerializerMapping::iterator bit = m_BinarySerializerMapping.begin();
	while ( bit != m_BinarySerializerMapping.end()) {
		delete (*bit).second;
		++bit;
	}
	m_Database.saveDB();
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
	sprintf(buffer,"%s\\%s.%s",converter->getResourceDirectory(),fileName,converter->getEnding());
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
		watch.binary = false;
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
	}
	else {
		LOGEC("AssetCompiler") << "Cannot find file: " << buffer;
	}
}

void AssetCompiler::update() {
	for ( size_t i = 0; i < m_WatchList.size(); ++i ) {
		FileWatch* watch = &m_WatchList[i];
		if ( file::compareFileTime(watch->jsonName,watch->fileTime)) {
			m_Database.update(watch->jsonName,watch->fileTime);
			LOGC("AssetCompiler") << "Reloading file: " << watch->jsonName;	
			if ( watch->binary ) {
				// do something here
				file::copyFile(watch->jsonName,watch->binaryName);
				renderer::reloadTextureBinary(watch->binaryName);
			}
			else {
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
				}
				else {
					LOGEC("AssetCompiler") << "Cannot reload binary file " << buffer;
				}
			}			
			file::getFileTime(watch->jsonName,watch->fileTime);
		}
	}
}

int AssetCompiler::loadBinary(const char* fileName,uint32 type) {
	assert( type != UINT32_MAX );
	// check if we support the type
	assert(m_BinarySerializerMapping.find(type) != m_BinarySerializerMapping.end());
	BinarySerializer* serializer = m_BinarySerializerMapping[type];
	char buffer[256];
	sprintf(buffer,"%s\\%s",serializer->getResourceDirectory(),fileName);
	if ( file::fileExists(buffer) ) {
		IdString hash = string::murmur_hash(buffer);
		char bb[256];
		sprintf(bb,"data\\%u",hash);
#ifdef DEBUG
		if ( m_Database.hasChanged(buffer) ) {
			LOGC("AssetCompiler") << "copying file " << buffer;		
			file::copyFile(buffer,bb);
			LOGC("AssetCompiler") << "file copied to " << bb;
		}	
		FileWatch watch;
		watch.hash = hash;
		strcpy(watch.jsonName,buffer);
		strcpy(watch.binaryName,bb);
		watch.binary = true;
		watch.type = type;
		file::getFileTime(buffer,watch.fileTime);
		m_WatchList.push_back(watch);
		LOGC("AssetCompiler") << "file watch created";
		m_Database.add(buffer,watch.fileTime);
#endif
		// load file
		LOGC("AssetCompiler") << "loading binary file: " << bb;

		
		return serializer->load(bb);

	}
	else {
		LOGEC("AssetCompiler") << "Cannot find file: " << buffer;
		return -1;
	}
	
}

static AssetCompiler compiler;

namespace assets {

	void load(const char* fileName, Serializer* serializer, uint32 type) {
		compiler.load(fileName, serializer, type);
	}

	uint32 registerConverter(Converter* converter) {
		return compiler.registerConverter(converter);
	}

	void update() {
		compiler.update();
	}

	void loadParticleSystem(const char* fileName, ParticleManager* particleManager) {
		compiler.load(fileName, particleManager, CVT_PARTICLEMANAGER);
	}

	void loadSpriteTemplates(const char* fileName) {
		compiler.load(fileName, renderer::getSpriteTemplates(), CVT_SPRITE);
	}
	

	int loadShader(const char* fileName) {
		int id = renderer::createEmptyShader(fileName);
		Shader* s = renderer::getShader(id);
		compiler.load(fileName, s, CVT_SHADER);		
		return id;
	}
	
}

}
