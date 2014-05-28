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

namespace ds {

AssetCompiler::AssetCompiler() {
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
	m_CustomIndex = 100;
}


AssetCompiler::~AssetCompiler() {
	ConverterMapping::iterator it = m_Mapping.begin();
	while ( it != m_Mapping.end() ) {
		delete (*it).second;
		++it;
	}
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
	load("content\\resources",fileName,serializer,type);
}

void AssetCompiler::load(const char* dir,const char* fileName,Serializer* serializer,uint32 type) {
	assert( type != UINT32_MAX );
	char buffer[256];
	sprintf(buffer,"%s\\%s.json",dir,fileName);
#ifdef DEBUG
	LOGC("AssetCompiler") << "converting file " << buffer;
	// check if we support the type
	assert(m_Mapping.find(type) != m_Mapping.end());
	Converter* converter = m_Mapping[type];
	// convert json to binary file
	converter->convert(fileName);
	FileWatch watch;
	strcpy(watch.jsonName,buffer);
	strcpy(watch.binaryName,fileName);
	watch.serializer = serializer;
	watch.type = type;
	file::getFileTime(buffer,watch.fileTime);
	m_WatchList.push_back(watch);
#endif
	// load file
	BinaryLoader loader;
	sprintf(buffer,"data\\%s.dsb",fileName);
	int signature[] = {0,8,15};
	if ( loader.open(buffer,signature,3) == 0 ) {
		serializer->load(&loader);
	}
	loader.close();
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
			LOGC("AssetCompiler") << "Reloading file: " << watch->jsonName;	
			assert(m_Mapping.find(watch->type) != m_Mapping.end());
			Converter* converter = m_Mapping[watch->type];
			// convert json to binary file
			converter->convert(watch->binaryName);
			BinaryLoader loader;
			char buffer[256];
			sprintf(buffer,"data\\%s.dsb",watch->binaryName);
			int signature[] = {0,8,15};
			if ( loader.open(buffer,signature,3) == 0 ) {
				watch->serializer->load(&loader);
			}
			loader.close();
			file::getFileTime(watch->jsonName,watch->fileTime);
		}
	}
}

}
